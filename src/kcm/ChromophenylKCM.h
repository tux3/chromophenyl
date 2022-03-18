#pragma once

#include <KCModule>
#include "ui_ChromophenylKCM.h"

class ChromophenylKCM : public KCModule {
    Q_OBJECT

public:
    explicit ChromophenylKCM(QWidget* parent = nullptr, const QVariantList& args = {});
    ~ChromophenylKCM() override;

    void save() override;

private:
    Ui::ChromophenylKCM* m_ui;
};
