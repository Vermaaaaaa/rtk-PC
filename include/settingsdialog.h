#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QObject>
#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include "settingsmanager.h"
#include "themesettingspage.h"

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(SettingsManager *Settings, QWidget *parent = nullptr);
    void setCurrentRow(int row);
    int getCurrentRow();
    void applyChanges(ThemeSettingsPage *themeSettingsPage);



private:
    int currentRow = 0;
    QListWidget *listWidget;
    QStackedWidget *stackedWidget;
    SettingsManager *settings;
    QPushButton *applyButton;

signals:
    void generatePlotsRequested(const QString &filePath,
                                bool rollTime, bool pitchTime, bool yawTime,
                                bool ecefXTime, bool ecefYTime, bool ecefZTime,
                                bool latTime, bool lonTime, bool altTime,
                                bool ecef3D, bool rpy3D, bool geodetic3D);

    void requestAnimationFile(const QString &filePath);
    void generateAnimationRequested();



};

#endif // SETTINGSDIALOG_H
