#include "include/nav_history_chart_view.h"

#include <QDateTime>
#include <QDebug>

NavHistoryChartView::NavHistoryChartView(QWidget *parent) : QtCharts::QChartView(parent) {
    chart = new QtCharts::QChart();
    line_series = new QtCharts::QLineSeries(chart);
    scatter_series = new QtCharts::QScatterSeries(chart);
    axis_x = new QtCharts::QDateTimeAxis(this);
    axis_y = new QtCharts::QValueAxis(this);

    chart->addSeries(line_series);
    chart->addSeries(scatter_series);
    chart->legend()->hide();

    axis_x->setFormat("MM-dd");
    chart->addAxis(axis_x, Qt::AlignBottom);
    axis_x->setTickCount(10);
    line_series->attachAxis(axis_x);

    axis_y->setLabelFormat("%.2f");
    axis_y->setTickCount(10);
    chart->addAxis(axis_y, Qt::AlignLeft);
    line_series->attachAxis(axis_y);

    scatter_series->attachAxis(axis_x);
    scatter_series->attachAxis(axis_y);
    scatter_series->setMarkerShape(QtCharts::QScatterSeries::MarkerShapeCircle);//圆形的点
    scatter_series->setBorderColor(QColor(35, 159, 223)); //离散点边框颜色
    scatter_series->setBrush(QBrush(QColor(255, 255, 255)));//离散点背景色
    scatter_series->setMarkerSize(6); //离散点大小

    this->setChart(chart);
    this->setRenderHint(QPainter::Antialiasing);
    this->setMinimumWidth(600);

    connect(scatter_series, &QtCharts::QScatterSeries::hovered, this, &NavHistoryChartView::point_hovered);
}

NavHistoryChartView::~NavHistoryChartView() {
    delete chart;
}

void NavHistoryChartView::append_data_to_chart(const QList<QPointF> &list, const QDateTime &min_time,
                                               const QDateTime &max_time, double min_nav, double max_nav) {
    line_series->append(list);
    scatter_series->append(list);
    axis_x->setRange(min_time, max_time);
    axis_y->setRange(min_nav - 0.3, max_nav + 0.3);
}

void NavHistoryChartView::point_hovered(const QPointF &point, bool state) {
    if (tooltip_label == nullptr) {
        tooltip_label = new QLabel(this);
        tooltip_label->setStyleSheet(
                "color: white;"
                "background-color: rgba(0, 0, 0, 0.80);"
                "padding: 5px;"
        );
    }
    if (state) {
        QDateTime date = QDateTime::fromMSecsSinceEpoch(point.x());
        QString text = QString("%1\n当日净值：%2").arg(date.toString("yyyy-MM-dd"), QString::number(point.y()));
        QPoint cur_pos = mapFromGlobal(QCursor::pos());
        tooltip_label->setText(text);
        tooltip_label->move(cur_pos.x() - tooltip_label->width() / 2, int(cur_pos.y() - tooltip_label->height() * 1.5));
        tooltip_label->show();
    } else {
        tooltip_label->hide();
    }
}
