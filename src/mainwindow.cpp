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
#include "OpenGLWidget.h"
#include <QPushButton>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include "PlotController.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    settings = new SettingsManager();

    applyStyleSheet(settings->guiSettings().getTheme());



    menusBar = menuBar()->addMenu("Settings");

    sessionManagmentAction = new QAction("Plots", this);
    rendererAction= new QAction("Animation", this);
    themesAction    = new QAction("Themes", this);

    menusBar->addAction(sessionManagmentAction);
    menusBar->addAction(rendererAction);
    menusBar->addAction(themesAction);

    connect(sessionManagmentAction, &QAction::triggered, this, &MainWindow::OpenSessionManagementSettings);
    connect(rendererAction, &QAction::triggered, this, &MainWindow::OpenRendererSettings);
    connect(themesAction, &QAction::triggered, this, &MainWindow::OpenThemeSettings);

    menusBar = menuBar()->addMenu("Session");

    startSessionAction = new QAction("Start Session", this);
    stopSessionAction = new QAction("Stop Session", this);
    pauseSessionAction = new QAction("Pause Session", this);

    menusBar->addAction(startSessionAction);
    menusBar->addAction(stopSessionAction);
    menusBar->addAction(pauseSessionAction);

    connect(startSessionAction, &QAction::triggered, this, &MainWindow::startSession);
    connect(stopSessionAction, &QAction::triggered, this, &MainWindow::stopSession);
    connect(pauseSessionAction, &QAction::triggered, this, &MainWindow::pauseUSBData);


    startSessionAction->setEnabled(true);
    stopSessionAction->setEnabled(false);
    pauseSessionAction->setEnabled(false);




    settingsDialog = new SettingsDialog(settings, this);

    auto* ctrl = new SessionController(this);
    connect(settingsDialog,
            &SettingsDialog::generatePlotsRequested,
            ctrl,
            &SessionController::generatePlots);

    connect(settingsDialog, &SettingsDialog::requestAnimationFile, this, &MainWindow::openAnimationFile);

    connect(settingsDialog, &SettingsDialog::generateAnimationRequested, this, &MainWindow::processNextDiff);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    splitter = new QSplitter(Qt::Vertical, this);



    openGL = new OpenGLWidget(this);



    tabWidget = new QTabWidget(this);
    debugConsole = new QPlainTextEdit(this);
    debugConsole->setReadOnly(true);
    debugConsole->setMinimumHeight(150);

    tabWidget->addTab(debugConsole, "Debug Console");

    splitter->addWidget(openGL);
    splitter->addWidget(tabWidget);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(splitter);
    centralWidget->setLayout(mainLayout);


    logMessage("RTKVISPOS Active");







    showMaximized();
}

void MainWindow::openAnimationFile(const QString &filePath){
    m_diffLines.clear();
    m_diffIndex = 0;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open RPY diffs file!";
    } else {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty())
                m_diffLines.append(line);
        }
        file.close();
    }

}

void MainWindow::processNextDiff()
{
    if (m_diffIndex >= m_diffLines.size())
        return;                // done

    // parse this line:
    QStringList parts = m_diffLines.at(m_diffIndex++).split(',', Qt::SkipEmptyParts);
    if (parts.size() < 4) {
        qWarning() << "Bad line:" << parts;
    } else {
        // your existing indices:
        double rollDeg  = parts[0].toDouble();
        double pitchDeg = parts[1].toDouble();
        double yawDeg   = parts[2].toDouble();

        // convert:
        double rollRad  = qDegreesToRadians(rollDeg);
        double pitchRad = qDegreesToRadians(pitchDeg);
        double yawRad   = qDegreesToRadians(yawDeg);

        rollRad *= -1;

        logMessage(QString("Roll: %1").arg(rollRad));
        logMessage(QString("Pitch: %1").arg(pitchRad));
        logMessage(QString("Yaw: %1").arg(yawRad));

        // apply (you already have this):
        openGL->updateRotation(rollRad, pitchRad, yawRad);
    }

    // schedule next in 1 s:
    QTimer::singleShot(1000, this, &MainWindow::processNextDiff);
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
    if (usbReader) {
        usbReader->stopReading();
        usbThread->quit();
        usbThread->wait();
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    settings->saveSettings();
    closeUSBData();
    QMainWindow::closeEvent(event);
}


bool MainWindow::setupUSB() {

    usbReader = new USBReader;
    usbThread = new QThread(this);


    usbReader->moveToThread(usbThread);

    if (!usbReader->autoDetectPort(/*VID*/0x0483, /*PID*/0x5740, /*baud*/QSerialPort::Baud115200)) {
        QMessageBox::warning(
            this,
            tr("No STM32 VCP"),
            tr("Could not find any USB device with VID 0483 & PID 5740. "
               "Make sure your board is plugged in and the drivers are installed.")
            );
        usbReader->deleteLater();
        usbThread->deleteLater();
        return false;
    }


    connect(usbThread, &QThread::started, usbReader, &USBReader::startReading);


    connect(this, &MainWindow::destroyed, usbReader, &USBReader::stopReading);
    connect(usbReader, &USBReader::usbClosed, usbThread, &QThread::quit);
    connect(usbReader, &USBReader::usbClosed, usbReader, &USBReader::deleteLater);
    connect(usbThread, &QThread::finished, usbThread, &QThread::deleteLater);


    connect(usbReader, &USBReader::usbData,
            this, &MainWindow::handleUSBData,
            Qt::QueuedConnection);
    connect(usbReader, &USBReader::usbError,
            this, &MainWindow::handleUSBError,
            Qt::QueuedConnection);

    usbThread->start();

    return true;

}


void MainWindow::handleUSBData(OutputData data){


    openGL->updateRotation(data.roll, data.pitch, data.yaw);

    if (!sessionActive) return;


    QJsonObject obj;
    obj["time"]      = QDateTime::currentMSecsSinceEpoch();

    // ============= Antenna 1 =============================

    obj["ant1_lon"] = data.antenna1_lon;
    obj["ant1_lat"] = data.antenna1_lat;
    obj["ant1_heightMSL"] = data.antenna1_heightMSL;

    obj["ant1_ECEFX"] = data.antenna1_ECEFX;
    obj["ant1_ECEFY"] = data.antenna1_ECEFY;
    obj["ant1_ECEFZ"] = data.antenna1_ECEFZ;
    obj["ant1_posAcc"] = data.antenna1_posAcc;


    obj["ant1_ECEFX_VEL"] = data.antenna1_ECEFX_VEL;
    obj["ant1_ECEFY_VEL"] = data.antenna1_ECEFY_VEL;
    obj["ant1_ECEFZ_VEL"] = data.antenna1_ECEFZ_VEL;
    obj["ant1_sAcc"] = data.antenna1_sAcc;

    // ============= Antenna 2 =============================

    obj["ant2_lon"] = data.antenna2_lon;
    obj["ant2_lat"] = data.antenna2_lat;
    obj["ant2_heightMSL"] = data.antenna2_heightMSL;

    obj["ant2_ECEFX"] = data.antenna2_ECEFX;
    obj["ant2_ECEFY"] = data.antenna2_ECEFY;
    obj["ant2_ECEFZ"] = data.antenna2_ECEFZ;
    obj["ant2_posAcc"] = data.antenna2_posAcc;


    obj["ant2_ECEFX_VEL"] = data.antenna2_ECEFX_VEL;
    obj["ant2_ECEFY_VEL"] = data.antenna2_ECEFY_VEL;
    obj["ant2_ECEFZ_VEL"] = data.antenna2_ECEFZ_VEL;
    obj["ant2_sAcc"] = data.antenna2_sAcc;

    // ============= Antenna 3 =============================

    obj["ant3_lon"] = data.antenna3_lon;
    obj["ant3_lat"] = data.antenna3_lat;
    obj["ant3_heightMSL"] = data.antenna3_heightMSL;

    obj["ant3_ECEFX"] = data.antenna3_ECEFX;
    obj["ant3_ECEFY"] = data.antenna3_ECEFY;
    obj["ant3_ECEFZ"] = data.antenna3_ECEFZ;
    obj["ant3_posAcc"] = data.antenna3_posAcc;


    obj["ant3_ECEFX_VEL"] = data.antenna3_ECEFX_VEL;
    obj["ant3_ECEFY_VEL"] = data.antenna3_ECEFY_VEL;
    obj["ant3_ECEFZ_VEL"] = data.antenna3_ECEFZ_VEL;
    obj["ant3_sAcc"] = data.antenna3_sAcc;

    // ============= System =============================

    obj["system_roll"]   = data.roll;
    obj["system_pitch"]  = data.pitch;
    obj["system_yaw"]    = data.yaw;

    obj["system_ECEFX"] = data.system_ECEFX;
    obj["system_ECEFY"] = data.system_ECEFY;
    obj["system_ECEFZ"] = data.system_ECEFZ;

    obj["system_lon"] = data.system_lon;
    obj["system_lat"] = data.system_lat;
    obj["system_heightMSL"] = data.system_heightMSL;


    QJsonDocument doc(obj);
    sessionFile.write(doc.toJson(QJsonDocument::Compact));
    sessionFile.write("\n");


    logMessage(QString("============================= Antenna 1 ============================="));
    // ============= Antenna 1 =============================

    logMessage(QString("Latitude: %1").arg(data.antenna1_lat));
    logMessage(QString("Longitude: %1").arg(data.antenna1_lon));
    logMessage(QString("heightMSL: %1").arg(data.antenna1_heightMSL));

    logMessage(QString("ECEFX: %1").arg(data.antenna1_ECEFX));
    logMessage(QString("ECEFY: %1").arg(data.antenna1_ECEFY));
    logMessage(QString("ECEFZ: %1").arg(data.antenna1_ECEFZ));
    logMessage(QString("PosAcc: %1").arg(data.antenna1_posAcc));

    logMessage(QString("ECEFX_VEL: %1").arg(data.antenna1_ECEFX_VEL));
    logMessage(QString("ECEFY_VEL: %1").arg(data.antenna1_ECEFY_VEL));
    logMessage(QString("ECEFZ_VEL: %1").arg(data.antenna1_ECEFZ_VEL));
    logMessage(QString("sAcc: %1").arg(data.antenna1_sAcc));


    logMessage(QString("============================= Antenna 2 ============================="));

// ============= Antenna 2 =============================
    logMessage(QString("Latitude: %1").arg(data.antenna2_lat));
    logMessage(QString("Longitude: %1").arg(data.antenna2_lon));
    logMessage(QString("heightMSL: %1").arg(data.antenna2_heightMSL));

    logMessage(QString("ECEFX: %1").arg(data.antenna2_ECEFX));
    logMessage(QString("ECEFY: %1").arg(data.antenna2_ECEFY));
    logMessage(QString("ECEFZ: %1").arg(data.antenna2_ECEFZ));
    logMessage(QString("PosAcc: %1").arg(data.antenna2_posAcc));

    logMessage(QString("ECEFX_VEL: %1").arg(data.antenna2_ECEFX_VEL));
    logMessage(QString("ECEFY_VEL: %1").arg(data.antenna2_ECEFY_VEL));
    logMessage(QString("ECEFZ_VEL: %1").arg(data.antenna2_ECEFZ_VEL));
    logMessage(QString("sAcc: %1").arg(data.antenna2_sAcc));

    logMessage(QString("============================= Antenna 3 ============================="));
// ============= Antenna 3 =============================

    logMessage(QString("Latitude: %1").arg(data.antenna3_lat));
    logMessage(QString("Longitude: %1").arg(data.antenna3_lon));
    logMessage(QString("heightMSL: %1").arg(data.antenna3_heightMSL));

    logMessage(QString("ECEFX: %1").arg(data.antenna3_ECEFX));
    logMessage(QString("ECEFY: %1").arg(data.antenna3_ECEFY));
    logMessage(QString("ECEFZ: %1").arg(data.antenna3_ECEFZ));
    logMessage(QString("PosAcc: %1").arg(data.antenna3_posAcc));

    logMessage(QString("ECEFX_VEL: %1").arg(data.antenna3_ECEFX_VEL));
    logMessage(QString("ECEFY_VEL: %1").arg(data.antenna3_ECEFY_VEL));
    logMessage(QString("ECEFZ_VEL: %1").arg(data.antenna3_ECEFZ_VEL));
    logMessage(QString("sAcc: %1").arg(data.antenna3_sAcc));
    logMessage(QString("============================= System ============================="));
    // ============= System =============================

    logMessage(QString("ECEFX: %1").arg(data.system_ECEFX));
    logMessage(QString("ECEFY: %1").arg(data.system_ECEFY));
    logMessage(QString("ECEFZ: %1").arg(data.system_ECEFZ));

    logMessage(QString("Latitude: %1").arg(data.system_lat));
    logMessage(QString("Longitude: %1").arg(data.system_lon));
    logMessage(QString("heightMSL: %1").arg(data.system_heightMSL));


    logMessage(QString("Roll: %1").arg(data.roll));
    logMessage(QString("Pitch: %1").arg(data.pitch));
    logMessage(QString("Yaw: %1").arg(data.yaw));

}


void MainWindow::closeUSBData(){
    if(!usbReader){return;}


    USB_STATE usbState = usbReader->get_USBState();

    if(usbState == CLOSED){
        return;
    }

    logMessage("USB Data Closed");
    usbReader->stopReading();

}

void MainWindow::pauseUSBData(){
    if(!usbReader){return;}


    USB_STATE usbState = usbReader->get_USBState();
    switch(usbState){
    case RUNNING:
        logMessage("USB Data Paused");
        break;
    case PAUSED:
        logMessage("USB Data Unpaused");
        break;
    default:
        break;
    }
    usbReader->pauseReading();

    stopSessionAction->setEnabled(true);
    startSessionAction->setEnabled(false);
    pauseSessionAction->setEnabled(true);

}

void MainWindow::handleUSBError(const QString &err) {
    QMessageBox::critical(this, "USB Error", err);
}

void MainWindow::startSession() {
    if (sessionActive) return;

    bool usb_status = setupUSB();

    if(!usb_status) { return; }


    QString dir = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    QString fname = QStringLiteral("%1/session_%2.ndjson")
                        .arg(dir,
                             QDateTime::currentDateTime()
                                 .toString("yyyyMMdd_hhmmss"));
    sessionFile.setFileName(fname);

    if (!sessionFile.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Cannot start session"),
                             tr("Failed to open %1 for writing").arg(fname));
        return;
    }

    sessionActive = true;
    logMessage(QString("Session started: %1").arg(fname));


    startSessionAction->setEnabled(false);
    stopSessionAction->setEnabled(true);
    pauseSessionAction->setEnabled(true);
}

void MainWindow::stopSession() {
    if (!sessionActive) return;


    if (usbReader) {
        closeUSBData();
    }



    if (!sessionFile.commit()) {
        QMessageBox::warning(this, tr("Save failed"),
                             tr("Could not save session file."));
    } else {
        logMessage(tr("Session saved to %1").arg(sessionFile.fileName()));
    }

    sessionActive = false;

    stopSessionAction ->setEnabled(false);
    pauseSessionAction->setEnabled(false);
    startSessionAction->setEnabled(true);
}
