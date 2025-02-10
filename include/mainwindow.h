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



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void applyStyleSheet(const QString &styleSheetPath);
    void logMessage(const QString &message);

private:
    SettingsDialog *settingsDialog;
    QPlainTextEdit *debugConsole;
    QSplitter *splitter;
    SettingsManager *settings;

private:
    void closeEvent(QCloseEvent *event) override;



private slots:
    void OpenSessionManagementSettings();
    void OpenRendererSettings();
    void OpenThemeSettings();
};
#endif // MAINWINDOW_H
