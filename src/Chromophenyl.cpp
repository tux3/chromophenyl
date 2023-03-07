#include <kwinglplatform.h>

#include "Chromophenyl.h"
#include "ChromophenylConfig.h"

using KWin::ShaderManager;
using KWin::ShaderBinder;
using KWin::GLFramebuffer;
using KWin::effects;

Chromophenyl::Chromophenyl()
        : m_inited(false),
          m_valid(true),
          m_active(true),
          m_refreshInterval(100),
          m_forceRepaintTimer(nullptr),
          m_lastPresentTime(std::chrono::milliseconds::zero()),
          m_texture(nullptr),
          m_fbo(nullptr),
          m_vbo(nullptr),
          m_shader(nullptr) {
    m_forceRepaintTimer.setSingleShot(false);
    m_forceRepaintTimer.setInterval(m_refreshInterval);
    connect(&m_forceRepaintTimer, &QTimer::timeout, this, &Chromophenyl::scheduleRepaint);

    reconfigure(ReconfigureAll);
}

Chromophenyl::~Chromophenyl() {
    delete m_texture;
    delete m_fbo;
    delete m_vbo;
}

bool Chromophenyl::initializeFramebuffer(QSize screenSize) {
    int texw = screenSize.width();
    int texh = screenSize.height();

    // Create texture and render target
    if (m_texture)
        delete m_texture;
    m_texture = new KWin::GLTexture(GL_RGBA8, texw, texh, 1);
    m_texture->setFilter(GL_LINEAR);
    m_texture->setWrapMode(GL_CLAMP_TO_EDGE);

    if (m_fbo)
        delete m_fbo;
    m_fbo = new GLFramebuffer(m_texture);
    if (!m_fbo->valid()) {
        qCritical() << "Chromophenyl failed to create FBO render target!";
        return false;
    }

    if (m_vbo)
        delete m_vbo;
    m_vbo = new KWin::GLVertexBuffer(KWin::GLVertexBuffer::Static);
    QVector<float> verts;
    QVector<float> texcoords;
    texcoords << screenSize.width() << 0.0;
    verts << screenSize.width() << 0.0;
    texcoords << 0.0 << 0.0;
    verts << 0.0 << 0.0;
    texcoords << 0.0 << screenSize.height();
    verts << 0.0 << screenSize.height();
    texcoords << 0.0 << screenSize.height();
    verts << 0.0 << screenSize.height();
    texcoords << screenSize.width() << screenSize.height();
    verts << screenSize.width() << screenSize.height();
    texcoords << screenSize.width() << 0.0;
    verts << screenSize.width() << 0.0;
    m_vbo->setData(6, 2, verts.constData(), texcoords.constData());

    return true;
}

bool Chromophenyl::loadData() {
    Q_INIT_RESOURCE(chromophenyl);
    m_inited = true;

    const QSize screenSize = effects->virtualScreenSize();
    if (!initializeFramebuffer(screenSize))
        return false;

    m_shader = ShaderManager::instance()->generateShaderFromFile(KWin::ShaderTrait::MapTexture, QString(),
                                                                 QStringLiteral(":/effects/chromophenyl/shader.frag"));
    if (!m_shader->isValid()) {
        qCritical() << "The chromophenyl shader failed to load!";
        return false;
    }
    ShaderBinder binder(m_shader.get());
    m_shader->setUniform("u_textureSize", QVector2D(screenSize.width(), screenSize.height()));
    qDebug() << "Chromophenyl shader loaded!";
    return true;
}

void Chromophenyl::reconfigure(ReconfigureFlags flags) {
    Q_UNUSED(flags)

    ChromophenylConfig::self()->read();
    if (!m_inited)
        m_valid = loadData();

    m_refreshInterval = 1000. / ChromophenylConfig::minimumFPS();
    m_forceRepaintTimer.setInterval(m_refreshInterval);

    ShaderBinder binder(m_shader.get());
    m_shader->setUniform("u_colAlpha", (float) ChromophenylConfig::intensity());
    m_shader->setUniform("u_globalSpeed", (float) ChromophenylConfig::speed());

    if (m_valid) {
        if (m_active)
            m_forceRepaintTimer.start();
        else
            m_forceRepaintTimer.stop();
    }
}

bool Chromophenyl::isActive() const {
    return m_valid && m_active;
}

bool Chromophenyl::supported() {
    return KWin::effects->isOpenGLCompositing();
}

void Chromophenyl::prePaintScreen(KWin::ScreenPrePaintData &data, std::chrono::milliseconds presentTime) {
    if (m_valid && m_active) {
        m_lastPresentTime = presentTime;
        data.mask |= PAINT_SCREEN_TRANSFORMED;
    }

    effects->prePaintScreen(data, presentTime);
}

void Chromophenyl::paintScreen(int mask, const QRegion &region,
                               KWin::ScreenPaintData &data) {
    if (m_valid && m_active) {
        const QSize screenSize = effects->virtualScreenSize();
        if (screenSize != m_texture->size() && !initializeFramebuffer(screenSize)) {
            return;
        }

        // Render to texture
        GLFramebuffer::pushFramebuffer(m_fbo);
        effects->paintScreen(mask, region, data);
        GLFramebuffer::popFramebuffer();

        // Use the shader
        ShaderBinder binder(m_shader.get());
        m_shader->setUniform("u_time", (float) (m_lastPresentTime.count() / 1000.));
        m_shader->setUniform("u_textureSize", QVector2D(screenSize.width(), screenSize.height()));
        m_shader->setUniform(KWin::GLShader::ModelViewProjectionMatrix, data.projectionMatrix());
        m_texture->bind();
        m_vbo->bindArrays();
        m_vbo->render(GL_TRIANGLES);
        m_vbo->unbindArrays();
        m_texture->unbind();
    }
}

void Chromophenyl::scheduleRepaint() {
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now);
    auto time_diff = now_ms.count() - m_lastPresentTime.count();

    if (time_diff > m_refreshInterval) {
        //qDebug() << "chromophenyl: no repaint in" << time_diff << "ms, forcing one";
        effects->addRepaintFull();
    }
}
