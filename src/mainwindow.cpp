#include "mainwindow.h"
#include "settingsmanager.h"
#include <QFile>
#include <QMenuBar>
#include <QApplication>
#include <QScreen>
#include <QDateTime>
#include <QTimer>
#include <QSplitter>
#include <QTabWidget>

#include <QTabWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 1. Initialize Settings
    settings = new SettingsManager();



    // 2. Create Menu Bar
    QMenu *settingsMenu = menuBar()->addMenu("Settings");

    QAction *sessionManagmentAction = new QAction("Session Management", this);
    QAction *rendererAction = new QAction("3D Renderer", this);
    QAction *themesAction = new QAction("Themes", this);

    settingsMenu->addAction(sessionManagmentAction);
    settingsMenu->addAction(rendererAction);
    settingsMenu->addAction(themesAction);

    connect(sessionManagmentAction, &QAction::triggered, this, &MainWindow::OpenSessionManagementSettings);
    connect(rendererAction, &QAction::triggered, this, &MainWindow::OpenRendererSettings);
    connect(themesAction, &QAction::triggered, this, &MainWindow::OpenThemeSettings);

    // 3. Create Settings Dialog
    settingsDialog = new SettingsDialog(settings, this);

    // 4. Create Central Widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 5. Create Splitter for Resizing
    QSplitter *splitter = new QSplitter(Qt::Vertical, this);

    // 6. Create 3D Renderer Widget
    QPlainTextEdit *temp = new QPlainTextEdit(this);

    // 7. Create Tab Widget for Debug Console
    QTabWidget *tabWidget = new QTabWidget(this);
    debugConsole = new QPlainTextEdit(this);
    debugConsole->setReadOnly(true);
    debugConsole->setMinimumHeight(150);

    // 8. Add Debug Console to Tab Widget
    tabWidget->addTab(debugConsole, "Debug Console");

    // 9. Add Widgets to Splitter
    splitter->addWidget(temp);
    splitter->addWidget(tabWidget);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

    // 10. Create Main Layout and Assign to Central Widget
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(splitter);
    centralWidget->setLayout(mainLayout);

    // 11. Log Initial Message
    logMessage("Hello World");

    // 12. Show Maximized Last
    showMaximized();
}



void MainWindow::OpenSessionManagementSettings(){
    settingsDialog->setCurrentRow(0);
    settingsDialog->show();
}

void MainWindow::OpenRendererSettings(){
    settingsDialog->setCurrentRow(1);
    settingsDialog->show();
}

void MainWindow::OpenThemeSettings(){
    settingsDialog->setCurrentRow(2);
    settingsDialog->show();
}

void MainWindow::applyStyleSheet(const QString &styleSheetPath) {
    QFile file(styleSheetPath);
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "Failed to load QSS file:" << styleSheetPath;
    }
}

void MainWindow::logMessage(const QString &message){
    QString timestamp = QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm:ss");
    debugConsole->appendPlainText(QString("[%1] %2").arg(timestamp, message));
}

MainWindow::~MainWindow()
{
    delete settings;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    settings->saveSettings(); // Save settings before exit
    QMainWindow::closeEvent(event);
}
