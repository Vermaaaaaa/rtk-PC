#include "mainwindow.h"
#include <QApplication>
#include <QIcon>
#include <QFile>
#include <QSurfaceFormat>


int main(int argc, char *argv[])
{

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

