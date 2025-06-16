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
#include "USBReader.h"
#include <QToolBar>
#include <QSaveFile>
#include "OpenGLWidget.h"
#include <QStringList>




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void applyStyleSheet(const themes_t theme);
    void logMessage(const QString &message);

private:
    bool setupUSB();
    QAction *sessionManagmentAction;
    QAction *rendererAction;
    QAction *themesAction;
    QAction *startSessionAction;
    QAction *stopSessionAction;
    QAction *pauseSessionAction;
    QAction *tempSessionAction;

    QTabWidget *tabWidget;

    QWidget *centralWidget;
    QMenu *menusBar;
    SettingsDialog *settingsDialog;

    QPlainTextEdit *debugConsole;
    QSplitter *splitter;
    SettingsManager *settings;
    QThread *usbThread = nullptr;
    USBReader *usbReader = nullptr;
    QToolBar *toolBar;
    QSaveFile sessionFile;
    bool sessionActive = false;

    OpenGLWidget *openGL;


    QStringList   m_diffLines;
    int           m_diffIndex = 0;





protected:
    void closeEvent(QCloseEvent *event) override;



private slots:
    void OpenSessionManagementSettings();
    void OpenRendererSettings();
    void OpenThemeSettings();
    void handleUSBData(OutputData data);
    void handleUSBError(const QString &err);
    void closeUSBData();
    void pauseUSBData();
    void startSession();
    void stopSession();

    void processNextDiff();
    void openAnimationFile(const QString &filePath);
};
#endif // MAINWINDOW_H
