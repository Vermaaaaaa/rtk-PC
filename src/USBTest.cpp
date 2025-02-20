#include "USBTest.h"
#include <QThread>
#include <QObject>
#include <QRandomGenerator>

void USBReader::startReading() {
    usbState = RUNNING;
    quint32 temp = 0;

    while (true) {
        mutex.lock();

        // Handle thread stopping properly
        if (usbState == CLOSED) {
            mutex.unlock();
            break;  // Exit the loop
        }

        // Pause the thread without holding the lock
        while (usbState == PAUSED) {
            emit usbPaused();
            cond.wait(&mutex);  // This releases the lock while waiting
            if (usbState == CLOSED) {
                mutex.lock();
                break;
            }
        }

        // Generate USB data only when running
        temp = QRandomGenerator::global()->generate();
        emit usbData(temp);

        mutex.unlock();  // Unlock before sleeping
        QThread::msleep(1000);  // Simulated delay
    }

    emit usbClosed();
}

void USBReader::stopReading() {
    mutex.lock();
    if(usbState == CLOSED){
        mutex.unlock();
        return;
    }
    usbState = CLOSED;
    cond.wakeAll();
    mutex.unlock();
}

void USBReader::pauseReading() {
    mutex.lock();
    if (usbState == RUNNING) {
        usbState = PAUSED;
    }
    else if (usbState == PAUSED) {
        usbState = RUNNING;
        cond.wakeAll();  // Resume thread if paused
    }
    mutex.unlock();
}

USB_STATE USBReader::get_USBState(){
    return usbState;
}
