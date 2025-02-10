#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QObject>
#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include "settingsmanager.h"
#include "renderersettingspage.h"
#include "themesettingspage.h"

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(SettingsManager *Settings, QWidget *parent = nullptr);
    void setCurrentRow(int row);
    int getCurrentRow();
    void applyChanges(RendererSettingsPage *rendererSettingsPage, ThemeSettingsPage *themeSettingsPage);



private:
    int currentRow = 0;
    QListWidget *listWidget;
    QStackedWidget *stackedWidget;
    SettingsManager *settings;
    QPushButton *applyButton;

};

#endif // SETTINGSDIALOG_H
