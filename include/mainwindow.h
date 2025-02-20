#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPlainTextEdit>
#include "settingsdialog.h"
#include <QSplitter>
#include "settingsmanager.h"
#include "guisettings.h"
#include <QThread>
#include "USBTest.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void applyStyleSheet(const themes_t theme);
    void logMessage(const QString &message);

private:
    void setupUSB();

    SettingsDialog *settingsDialog;
    QPlainTextEdit *debugConsole;
    QSplitter *splitter;
    SettingsManager *settings;
    QThread *usbThread;
    USBReader *usbReader;



protected:
    void closeEvent(QCloseEvent *event) override;



private slots:
    void OpenSessionManagementSettings();
    void OpenRendererSettings();
    void OpenThemeSettings();
    void handleUSBData(quint32 data);
    void closeUSBData();
    void pauseUSBData();
};
#endif // MAINWINDOW_H
