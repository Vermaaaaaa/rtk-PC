#ifndef USBREADER_H
#define USBREADER_H
#pragma once
#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <cstring>
#include <stdint.h>


typedef struct __attribute__((packed)) {

    uint8_t preamble;
    uint16_t length;

    // ==========  ANTENNA 1 =============== //

    // UBX-NAV-PVT Message Data
    int32_t  antenna1_lon;
    int32_t  antenna1_lat;
    int32_t  antenna1_heightMSL;

    // UBX-NAV-HPPOSECEF Message Data
    double antenna1_ECEFX;
    double antenna1_ECEFY;
    double antenna1_ECEFZ;
    double antenna1_posAcc;

    // UBX-NAV-VELECEF Message Data
    double antenna1_ECEFX_VEL;
    double antenna1_ECEFY_VEL;
    double antenna1_ECEFZ_VEL;
    double antenna1_sAcc;

    // ==========  ANTENNA 2 =============== //

    // UBX-NAV-PVT Message Data
    int32_t  antenna2_lon;
    int32_t  antenna2_lat;
    int32_t  antenna2_heightMSL;

    // UBX-NAV-HPPOSECEF Message Data
    double antenna2_ECEFX;
    double antenna2_ECEFY;
    double antenna2_ECEFZ;
    double antenna2_posAcc;

    // UBX-NAV-VELECEF Message Data
    double antenna2_ECEFX_VEL;
    double antenna2_ECEFY_VEL;
    double antenna2_ECEFZ_VEL;
    double antenna2_sAcc;

    // ==========  ANTENNA 3 =============== //

    // UBX-NAV-PVT Message Data
    int32_t  antenna3_lon;
    int32_t  antenna3_lat;
    int32_t  antenna3_heightMSL;

    // UBX-NAV-HPPOSECEF Message Data
    double antenna3_ECEFX;
    double antenna3_ECEFY;
    double antenna3_ECEFZ;
    double antenna3_posAcc;

    // UBX-NAV-VELECEF Message Data
    double antenna3_ECEFX_VEL;
    double antenna3_ECEFY_VEL;
    double antenna3_ECEFZ_VEL;
    double antenna3_sAcc;

    // ======= Roll, Pitch and Yaw ======= //
    double roll;
    double pitch;
    double yaw;

    // === System Location (Centroid) ===== //
    double system_ECEFX;
    double system_ECEFY;
    double system_ECEFZ;

    double system_lon;
    double system_lat;
    double system_heightMSL;

} OutputData;


inline static constexpr std::size_t HDR_SIZE  = 3;
inline static constexpr std::size_t FULL_SIZE = sizeof(OutputData);



typedef enum{
    RUNNING,
    PAUSED,
    CLOSED
}USB_STATE;

class USBReader : public QObject {
    Q_OBJECT
public:
    explicit USBReader(QObject *parent = nullptr);
    ~USBReader();
    USB_STATE get_USBState() const;

    bool autoDetectPort(quint16 targetVid = 0x0483, quint16 targetPid = 0x5740,
                        qint32 baud = QSerialPort::Baud115200);

public slots:
    void startReading();
    void stopReading();
    void pauseReading();



signals:
    void usbData(const OutputData &data);
    void usbClosed();
    void usbPaused();
    void usbError(const QString &errorString);

private slots:
    void handleReadyRead();
private:
    USB_STATE usbState = CLOSED;
    QMutex mutex;
    QWaitCondition cond;

    QSerialPort *serial = nullptr;
    QString      portName;
    qint32       baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity   parity;
    QSerialPort::StopBits stopBits;

    QByteArray buffer;

    std::vector<long long> delta_times;
    int delta_count = 0;

};



#endif // USBREADER_H
