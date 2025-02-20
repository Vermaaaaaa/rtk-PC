#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

/*TODO:
    Equal triangle settings
    Saving data from mc to files
    Render to Separate Windows
    Plot Graphs

*/
