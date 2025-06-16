// sessioncontroller.h
#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QVector3D>

struct SessionRow {
    qint64       tms;
    double       roll;
    double       pitch;
    double       yaw;
    double       x;
    double       y;
    double       z;
    double       lat;
    double       lon;
    double       height;
};

class SessionController : public QObject {
    Q_OBJECT
public:
    explicit SessionController(QObject *parent = nullptr);

public slots:
    void generatePlots(const QString &path,
                       bool rollTime, bool pitchTime, bool yawTime,
                       bool ecefXTime, bool ecefYTime, bool ecefZTime,
                       bool latTime, bool lonTime, bool altTime,
                       bool ecef3D, bool rpy3D, bool geodetic3D);
};
