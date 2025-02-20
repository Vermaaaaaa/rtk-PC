#ifndef USBTEST_H
#define USBTEST_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>

typedef enum{
    RUNNING,
    PAUSED,
    CLOSED
}USB_STATE;

class USBReader : public QObject {
    Q_OBJECT
public:
    explicit USBReader(QObject *parent = nullptr) : QObject(parent) {}
    USB_STATE get_USBState();

public slots:
    void startReading();
    void stopReading();
    void pauseReading();


signals:
    void usbData(const quint32 data);
    void usbClosed();
    void usbPaused();


private:
    USB_STATE usbState;
    QMutex mutex;
    QWaitCondition cond;
};


#endif // USBTEST_H
