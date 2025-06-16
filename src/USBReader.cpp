#include "USBReader.h"
#include <QThread>
#include <QObject>
#include <QRandomGenerator>

#include <qdebug.h>



USBReader::USBReader(QObject *parent)
    : QObject(parent)
{

}


USBReader::~USBReader() {
    stopReading();
}


void USBReader::startReading() {




    serial = new QSerialPort(this);
    serial->setPortName(portName);
    serial->setBaudRate(baudRate);
    serial->setDataBits(dataBits);
    serial->setParity(parity);
    serial->setStopBits(stopBits);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    connect(serial, &QSerialPort::errorOccurred, this,
            [this](QSerialPort::SerialPortError err){
                if (err != QSerialPort::NoError && err != QSerialPort::TimeoutError)
                    emit usbError(serial->errorString());
            });
    connect(serial, &QSerialPort::readyRead,
            this, &USBReader::handleReadyRead);

    if (!serial->open(QIODevice::ReadOnly)) {
        emit usbError(serial->errorString());
        return;
    }

    usbState = RUNNING;

}

void USBReader::handleReadyRead() {
    buffer.append(serial->readAll());

    while (buffer.size() >= FULL_SIZE) {

        if (static_cast<uint8_t>(buffer[0]) != 0xAA) {
            buffer.remove(0, 1);
            continue;
        }

        uint16_t len = *reinterpret_cast<const uint16_t*>(buffer.constData() + 1);
        if (len /*+ HDR_SIZE*/ != FULL_SIZE) {
            // bad length—skip this byte
            buffer.remove(0, 1);
            continue;
        }
        OutputData pkt;
        std::memcpy(&pkt, buffer.constData(), FULL_SIZE);
        buffer.remove(0, FULL_SIZE);
        {
            QMutexLocker locker(&mutex);
            if (usbState != RUNNING) break;
        }
        emit usbData(reinterpret_cast<const OutputData&>(pkt));
    }
}



void USBReader::stopReading() {

    QMutexLocker locker(&mutex);
    if (usbState == CLOSED){
        return;
    }

    usbState = CLOSED;


    if (serial) {
        serial->close();
        serial->deleteLater();
        serial = nullptr;
    }
    emit usbClosed();
}

void USBReader::pauseReading() {
    QMutexLocker locker(&mutex);
    if (usbState == RUNNING) {
        usbState = PAUSED;
        emit usbPaused();
    }
    else if (usbState == PAUSED) {
        usbState = RUNNING;
        emit usbPaused();
    }
}

USB_STATE USBReader::get_USBState() const{
    return usbState;
}


bool USBReader::autoDetectPort(quint16 targetVid, quint16 targetPid,
                               qint32 baud)
{
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        if (info.hasVendorIdentifier() &&
            info.hasProductIdentifier() &&
            info.vendorIdentifier() == targetVid &&
            info.productIdentifier() == targetPid)
        {
            portName = info.portName();
            baudRate = baud;
            dataBits = QSerialPort::Data8;
            parity   = QSerialPort::NoParity;
            stopBits = QSerialPort::OneStop;
            return true;
        }
    }
    return false;
}
