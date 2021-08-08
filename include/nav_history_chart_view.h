#ifndef NAVHISTORYCHARTVIEW_H
#define NAVHISTORYCHARTVIEW_H

#include <QObject>
#include <QToolTip>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QLabel>

class NavHistoryChartView : public QtCharts::QChartView {
public:
    explicit NavHistoryChartView(QWidget *parent = nullptr);

    ~NavHistoryChartView() override;

    void append_data_to_chart(const QList<QPointF> &list, const QDateTime &min_time, const QDateTime &max_time,
                              double min_nav, double max_nav);

private slots:

    void point_hovered(const QPointF &point, bool state);


private:
    QtCharts::QLineSeries *line_series;
    QtCharts::QScatterSeries *scatter_series;
    QtCharts::QChart *chart;
    QtCharts::QDateTimeAxis *axis_x;
    QtCharts::QValueAxis *axis_y;
    QLabel *tooltip_label;
};

#endif // NAVHISTORYCHARTVIEW_H
