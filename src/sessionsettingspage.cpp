#include "sessionsettingspage.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QGroupBox>


SessionSettingsPage::SessionSettingsPage(QWidget *parent)
    : QWidget(parent)
{
    auto *lay = new QVBoxLayout(this);


    openButton = new QPushButton(tr("Open Session…"), this);
    fileLabel  = new QLabel(tr("<no file selected>"), this);
    lay->addWidget(openButton);
    lay->addWidget(fileLabel);


    auto *plotsBox = new QGroupBox(tr("Plots to generate"), this);
    auto *plLay    = new QVBoxLayout;
    plotsBox->setLayout(plLay);
    cbRollTime   = new QCheckBox(tr("Roll vs Time"),    this);
    cbPitchTime  = new QCheckBox(tr("Pitch vs Time"),   this);
    cbYawTime    = new QCheckBox(tr("Yaw vs Time"),     this);
    cbEcefXTime   = new QCheckBox(tr("ECEF X vs Time"), this);
    cbEcefYTime   = new QCheckBox(tr("ECEF Y vs Time"), this);
    cbEcefZTime   = new QCheckBox(tr("ECEF Z vs Time"), this);
    cbLatTime    = new QCheckBox(tr("Latitude vs Time"), this);
    cbLonTime    = new QCheckBox(tr("Longitude vs Time"),this);
    cbAltTime    = new QCheckBox(tr("Altitude vs Time"), this);
    cbEcef3D     = new QCheckBox(tr("3D ECEF Path"),    this);
    cbRpy3D      = new QCheckBox(tr("3D R/P/Y Path"),   this);
    cbGeodetic3D = new QCheckBox(tr("3D Lat/Lon/Alt Path"), this);

    for (auto *cb : { cbRollTime, cbPitchTime, cbYawTime,
                     cbEcefXTime,cbEcefYTime,cbEcefZTime,  cbLatTime, cbLonTime,
                     cbAltTime, cbEcef3D, cbRpy3D, cbGeodetic3D })
    {
        plLay->addWidget(cb);
    }
    lay->addWidget(plotsBox);


    generateButton = new QPushButton(tr("Generate Plots"), this);
    lay->addWidget(generateButton);

    setControlsEnabled(false);


    connect(openButton,    &QPushButton::clicked, this, &SessionSettingsPage::onOpenSessionClicked);
    connect(generateButton,&QPushButton::clicked, this, &SessionSettingsPage::onGenerateClicked);
}

void SessionSettingsPage::setControlsEnabled(bool on) {
    QVector<QWidget*> widgets = {
        cbRollTime, cbPitchTime, cbYawTime,
        cbEcefXTime,cbEcefYTime,cbEcefZTime, cbLatTime, cbLonTime,
        cbAltTime, cbEcef3D, cbRpy3D,
        cbGeodetic3D, generateButton
    };
    for (QWidget *w : widgets) {
        w->setEnabled(on);
    }
}


void SessionSettingsPage::onOpenSessionClicked() {
    QString filter = tr("NDJSON Files (*.ndjson);;JSON Files (*.json)");
    QString fn = QFileDialog::getOpenFileName(this,
                                              tr("Select Session File"), QString(), filter);
    if (fn.isEmpty()) return;

    sessionFilePath = fn;
    fileLabel->setText(fn);
    setControlsEnabled(true);
}

void SessionSettingsPage::onGenerateClicked() {
    if (sessionFilePath.isEmpty()) return;

    emit generatePlotsRequested(
        sessionFilePath,
        cbRollTime->isChecked(),
        cbPitchTime->isChecked(),
        cbYawTime->isChecked(),
        cbEcefXTime->isChecked(),
        cbEcefYTime->isChecked(),
        cbEcefZTime->isChecked(),
        cbLatTime->isChecked(),
        cbLonTime->isChecked(),
        cbAltTime->isChecked(),
        cbEcef3D->isChecked(),
        cbRpy3D->isChecked(),
        cbGeodetic3D->isChecked()
        );
}
