#ifndef SESSIONSETTINGSPAGE_H
#define SESSIONSETTINGSPAGE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>


class SessionSettingsPage : public QWidget
{
    Q_OBJECT
public:
   explicit SessionSettingsPage(QWidget *parent = nullptr);

signals:
   void generatePlotsRequested(const QString &filePath,
                               bool rollTime, bool pitchTime, bool yawTime,
                               bool ecefXTime,bool ecefYTime,bool ecefZTime,
                               bool latTime, bool lonTime,
                               bool altTime, bool ecef3D,
                               bool rpy3D, bool geodetic3D);
private slots:
    void onOpenSessionClicked();
    void onGenerateClicked();

private:
    QPushButton *openButton;
    QLabel      *fileLabel;
    QCheckBox   *cbRollTime;
    QCheckBox   *cbPitchTime;
    QCheckBox   *cbYawTime;
    QCheckBox   *cbEcefXTime;
    QCheckBox   *cbEcefYTime;
    QCheckBox   *cbEcefZTime;
    QCheckBox   *cbLatTime;
    QCheckBox   *cbLonTime;
    QCheckBox   *cbAltTime;
    QCheckBox   *cbEcef3D;
    QCheckBox   *cbRpy3D;
    QCheckBox   *cbGeodetic3D;
    QPushButton *generateButton;
    QString      sessionFilePath;

    void setControlsEnabled(bool on);

};

#endif // SESSIONSETTINGSPAGE_H
