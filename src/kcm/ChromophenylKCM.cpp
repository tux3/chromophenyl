#include "ChromophenylKCM.h"
#include "ChromophenylConfig.h"
#include "kwineffects_interface.h"

ChromophenylKCM::ChromophenylKCM(QWidget* parent, const QVariantList& args)
    : KCModule(parent, args)
    , m_ui(new Ui::ChromophenylKCM)
{
    m_ui->setupUi(this);
    addConfig(ChromophenylConfig::self(), this);
    load();
}

ChromophenylKCM::~ChromophenylKCM()
{
    delete m_ui;
}

void ChromophenylKCM::save()
{
    KCModule::save();
    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
        QStringLiteral("/Effects"),
        QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("kwin4_effect_chromophenyl"));
}
