#include "PlotController.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>


#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>


#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/QScatter3DSeries>
#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/QScatterDataProxy>
#include <QtDataVisualization/QScatterDataItem>

#include <QVBoxLayout>




static QVector<SessionRow> loadSession(const QString &path) {
    QVector<SessionRow> rows;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return rows;
    while (!f.atEnd()) {
        auto line = f.readLine();
        auto doc  = QJsonDocument::fromJson(line);
        if (!doc.isObject()) continue;
        auto o = doc.object();

        SessionRow r;
        r.tms    = o.value("time").toVariant().toLongLong();
        r.roll   = o.value("system_roll").toDouble(qQNaN());
        r.pitch  = o.value("system_pitch").toDouble(qQNaN());
        r.yaw    = o.value("system_yaw").toDouble(qQNaN());
        r.x      = o.value("system_ECEFX").toDouble(qQNaN());
        r.y      = o.value("system_ECEFY").toDouble(qQNaN());
        r.z      = o.value("system_ECEFZ").toDouble(qQNaN());
        r.lat    = o.value("system_lat").toDouble(qQNaN());
        r.lon    = o.value("system_lon").toDouble(qQNaN());
        r.height = o.value("system_heightMSL").toDouble(qQNaN());
        rows.append(r);
    }
    return rows;
}


static void plot2D(const QString &title,
                   const QVector<QPair<double,double>> &pts,
                   const QString &xLabel,
                   const QString &yLabel)
{
    if (pts.isEmpty()) {
        QMessageBox::warning(nullptr, title,
                             QObject::tr("No data to plot %1").arg(title));
        return;
    }
    auto *series = new QLineSeries;
    series->setName(title);
    for (auto &p : pts)
        series->append(p.first, p.second);

    series->setPointLabelsVisible(true);
    series->setPointLabelsFormat(QLatin1String("@yPoint (@xPoint s)"));
    series->setPointLabelsClipping(false);
    series->setPointLabelsColor(Qt::black);
    series->setPointLabelsFont(QFont{"Arial", 8});
    series->setUseOpenGL(true);

    auto *chart = new QChart;
    chart->addSeries(series);
    chart->createDefaultAxes();



    auto hAxes = chart->axes(Qt::Horizontal);
    if (!hAxes.isEmpty()) {
        if (auto *axisX = qobject_cast<QValueAxis*>(hAxes[0]))
            axisX->setTitleText(xLabel);
    }


    auto vAxes = chart->axes(Qt::Vertical);
    if (!vAxes.isEmpty()) {
        if (auto *axisY = qobject_cast<QValueAxis*>(vAxes[0]))
            axisY->setTitleText(yLabel);
    }
    auto *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setRubberBand(QChartView::RectangleRubberBand);
    view->setInteractive(true);



    QDialog *dlg = new QDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle(title);
    QVBoxLayout *lay = new QVBoxLayout(dlg);
    lay->addWidget(view);
    dlg->resize(800, 600);
    dlg->show();
}

static void plot3D(const QString &title,
                   const QVector<QVector3D> &pts,
                   const QString &xLabel,
                   const QString &yLabel,
                   const QString &zLabel)
{
    if (pts.isEmpty()) {
        QMessageBox::warning(nullptr, title,
                             QObject::tr("No data to plot %1").arg(title));
        return;
    }
    auto *graph = new Q3DScatter;
    QWidget *container = QWidget::createWindowContainer(graph);

    auto *series = new QScatter3DSeries;
    auto *proxy  = series->dataProxy();
    auto *arr    = new QScatterDataArray;
    arr->reserve(pts.size());
    for (auto &v : pts)
        arr->append(QScatterDataItem(v));
    proxy->resetArray(arr);
    series->setItemSize(0.1f);
    graph->addSeries(series);

    graph->axisX()->setTitle(xLabel);
    graph->axisY()->setTitle(yLabel);
    graph->axisZ()->setTitle(zLabel);

    QDialog *dlg = new QDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle(title);
    QVBoxLayout *lay = new QVBoxLayout(dlg);
    lay->addWidget(container);
    dlg->resize(800, 600);
    dlg->show();
}



SessionController::SessionController(QObject *parent)
    : QObject(parent)
{}

void SessionController::generatePlots(const QString &path,
                                      bool rollTime, bool pitchTime, bool yawTime,
                                      bool ecefXTime, bool ecefYTime, bool ecefZTime,
                                      bool latTime, bool lonTime, bool altTime,
                                      bool ecef3D, bool rpy3D, bool geodetic3D)
{
    auto rows = loadSession(path);
    if (rows.isEmpty()) {
        QMessageBox::warning(nullptr, tr("Empty Session"),
                             tr("No data in %1").arg(path));
        return;
    }


    double t0 = rows[0].tms;


    QVector<QPair<double,double>> pts;
    if (rollTime) {
        pts.clear();
        for (auto &r : rows)
            if (!qIsNaN(r.roll))
                pts.append({ (r.tms - t0)/1000.0, r.roll });
        plot2D(tr("Roll vs Time"), pts, tr("Time (s)"), tr("Roll (rad)"));
    }
    if (pitchTime) {
        pts.clear();
        for (auto &r : rows)
            if (!qIsNaN(r.pitch))
                pts.append({ (r.tms - t0)/1000.0, r.pitch });
        plot2D(tr("Pitch vs Time"), pts, tr("Time (s)"), tr("Pitch (rad)"));
    }
    if (yawTime) {
        pts.clear();
        for (auto &r : rows)
            if (!qIsNaN(r.yaw))
                pts.append({ (r.tms - t0)/1000.0, r.yaw });
        plot2D(tr("Yaw vs Time"), pts, tr("Time (s)"), tr("Yaw (rad)"));
    }


    if (ecefXTime) {
        pts.clear();
        for (auto &r : rows)
            if (!qIsNaN(r.x))
                pts.append({ (r.tms - t0)/1000.0, r.x });
        plot2D(tr("ECEF X vs Time"), pts, tr("Time (s)"), tr("X (m)"));
    }
    if (ecefYTime) {
        pts.clear();
        for (auto &r : rows)
            if (!qIsNaN(r.y))
                pts.append({ (r.tms - t0)/1000.0, r.y });
        plot2D(tr("ECEF Y vs Time"), pts, tr("Time (s)"), tr("Y (m)"));
    }
    if (ecefZTime) {
        pts.clear();
        for (auto &r : rows)
            if (!qIsNaN(r.z))
                pts.append({ (r.tms - t0)/1000.0, r.z });
        plot2D(tr("ECEF Z vs Time"), pts, tr("Time (s)"), tr("Z (m)"));
    }

    if (latTime) {
        pts.clear();
        for (auto &r : rows)
            if (!qIsNaN(r.lat))
                pts.append({ (r.tms - t0)/1000.0, r.lat });
        plot2D(tr("Latitude vs Time"), pts, tr("Time (s)"), tr("Latitude (°)"));
    }
    if (lonTime) {
        pts.clear();
        for (auto &r : rows)
            if (!qIsNaN(r.lon))
                pts.append({ (r.tms - t0)/1000.0, r.lon });
        plot2D(tr("Longitude vs Time"), pts, tr("Time (s)"), tr("Longitude (°)"));
    }
    if (altTime) {
        pts.clear();
        for (auto &r : rows)
            if (!qIsNaN(r.height))
                pts.append({ (r.tms - t0)/1000.0, r.height });
        plot2D(tr("Altitude vs Time"), pts, tr("Time (s)"), tr("Height (m)"));
    }

    QVector<QVector3D> pts3;
    if (ecef3D) {
        pts3.clear();
        for (auto &r : rows)
            if (!qIsNaN(r.x) && !qIsNaN(r.y) && !qIsNaN(r.z))
                pts3.append({ float(r.x), float(r.y), float(r.z) });
        plot3D(tr("3D ECEF Path"), pts3, tr("X"), tr("Y"), tr("Z"));
    }

    if (rpy3D) {
        pts3.clear();
        for (auto &r : rows)
            if (!qIsNaN(r.roll) && !qIsNaN(r.pitch) && !qIsNaN(r.yaw))
                pts3.append({ float(r.roll), float(r.pitch), float(r.yaw) });
        plot3D(tr("3D R/P/Y Path"), pts3, tr("Roll"), tr("Pitch"), tr("Yaw"));
    }
    if (geodetic3D) {
        pts3.clear();
        for (auto &r : rows)
            if (!qIsNaN(r.lat) && !qIsNaN(r.lon) && !qIsNaN(r.height))
                pts3.append({ float(r.lon), float(r.lat), float(r.height) });
        plot3D(tr("3D Lat/Lon/Alt Path"), pts3,
               tr("Longitude"), tr("Latitude"), tr("Height"));
    }
}
