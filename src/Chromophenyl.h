#pragma once

#include <kwineffects.h>
#include <kwinglutils.h>
#include <QTimer>

class Chromophenyl : public KWin::Effect {
    Q_OBJECT

public:
    Chromophenyl();
    ~Chromophenyl() override;

    void reconfigure(ReconfigureFlags flags) override;

    bool isActive() const override;
    int requestedEffectChainPosition() const override;

    static bool supported();

    void prePaintScreen(KWin::ScreenPrePaintData& data, std::chrono::milliseconds presentTime) override;
    void paintScreen(int mask, const QRegion &region, KWin::ScreenPaintData& data) override;

private:
    bool loadData();

private slots:
    void scheduleRepaint();

private:
    bool m_inited;
    bool m_valid;
    bool m_active;
    int m_refreshInterval;
    QTimer m_forceRepaintTimer;
    std::chrono::milliseconds m_lastPresentTime;
    KWin::GLTexture *m_texture;
    KWin::GLRenderTarget *m_fbo;
    KWin::GLVertexBuffer *m_vbo;
    KWin::GLShader* m_shader;
};

inline int Chromophenyl::requestedEffectChainPosition() const
{
    return 90;
}
