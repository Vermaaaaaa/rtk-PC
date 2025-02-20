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

#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 1. Initialize Settings
    settings = new SettingsManager();

    applyStyleSheet(settings->guiSettings().getTheme());


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
    //QPlainTextEdit *temp = new QPlainTextEdit(this);

    QWidget *box = new QWidget(this);

    QVBoxLayout *tempLayout = new QVBoxLayout(this);

    QPushButton *temp = new QPushButton("CLOSE",this);
    QPushButton *temp2 = new QPushButton("PAUSE",this);

    connect(temp, &QPushButton::clicked, this, &MainWindow::closeUSBData);
    connect(temp2, &QPushButton::clicked, this, &MainWindow::pauseUSBData);

    tempLayout->addWidget(temp);
    tempLayout->addWidget(temp2);

    box->setLayout(tempLayout);



    // 7. Create Tab Widget for Debug Console
    QTabWidget *tabWidget = new QTabWidget(this);
    debugConsole = new QPlainTextEdit(this);
    debugConsole->setReadOnly(true);
    debugConsole->setMinimumHeight(150);

    // 8. Add Debug Console to Tab Widget
    tabWidget->addTab(debugConsole, "Debug Console");

    // 9. Add Widgets to Splitter
    splitter->addWidget(box);
    splitter->addWidget(tabWidget);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

    // 10. Create Main Layout and Assign to Central Widget
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(splitter);
    centralWidget->setLayout(mainLayout);

    // 11. Log Initial Message
    logMessage("USB Data Emulation Started");

    setupUSB();

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

void MainWindow::applyStyleSheet(const themes_t theme) {
    QString themePath;
    switch (theme) {
    case MATERIAL_DARK:
        themePath = ":/MaterialDark.qss";
        break;
    case MANJARO_MIX:
        themePath = ":/ManjaroMix.qss";
        break;
    case AMOLED:
        themePath = ":/AMOLED.qss";
        break;
    case AQUA:
        themePath = ":/Aqua.qss";
        break;
    case CONSOLE_STYLE:
        themePath = ":/ConsoleStyle.qss";
        break;
    case ELEGANT_DARK:
        themePath = ":/ElegantDark.qss";
        break;
    case UBUNTU:
        themePath = ":/Ubuntu.qss";
        break;
    }
    QFile file(themePath);
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "Failed to load QSS file:" << themePath;
    }
}

void MainWindow::logMessage(const QString &message){
    QString timestamp = QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm:ss");
    debugConsole->appendPlainText(QString("[%1] %2").arg(timestamp, message));
}

MainWindow::~MainWindow()
{
    delete settings;

    if (usbReader) {
        usbReader->stopReading();  // Ensure it stops before deletion
        usbThread->quit();
        usbThread->wait();
    }

    delete usbReader;
    delete usbThread;  // Clean up the thread after stopping
}

void MainWindow::closeEvent(QCloseEvent *event) {
    settings->saveSettings(); // Save settings before exit
    QMainWindow::closeEvent(event);
}


void MainWindow::setupUSB(){
    usbThread = new QThread(this);
    usbReader = new USBReader;

    usbReader->moveToThread(usbThread);

    connect(usbThread, &QThread::started, usbReader, &USBReader::startReading);
    connect(usbReader, &USBReader::usbData, this, &MainWindow::handleUSBData);
    connect(usbReader, &USBReader::usbClosed, usbThread, &QThread::quit);
    connect(usbThread, &QThread::finished, usbReader, &QObject::deleteLater);

    usbThread->start();


}

void MainWindow::handleUSBData(quint32 data)
{
    logMessage(QString("USB Data Received: %1").arg(data));
}


void MainWindow::closeUSBData()
{
    static bool closing = false;
    if(closing){return;}

    closing = true;

    logMessage("USB Data Emulation Closed");
    usbReader->stopReading();

    QTimer::singleShot(500, this, [this]() { closing = false; });
}

void MainWindow::pauseUSBData()
{
    USB_STATE usbState = usbReader->get_USBState();
    switch(usbState){
    case RUNNING:
        logMessage("USB Data Emulation Paused");
        break;
    case PAUSED:
        logMessage("USB Data Emulation Unpaused");
        break;
    default:
        break;
    }
    usbReader->pauseReading();
}

