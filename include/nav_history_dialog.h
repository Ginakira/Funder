#ifndef NAV_HISTORY_DIALOG_H
#define NAV_HISTORY_DIALOG_H

#include <QDialog>

#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QDateTime>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

namespace Ui {
class NavHistoryDialog;
}

class NavHistoryDialog : public QDialog {
Q_OBJECT

public:
    explicit NavHistoryDialog(const QString &code, const QString &name, QWidget *parent = nullptr);

    ~NavHistoryDialog() override;

private slots:

    void get_more_history();


private:
    static QVariantMap get_json_from_networker(const QString &url);

    void init_chart();

    Ui::NavHistoryDialog *ui;

    const QString code;

    const QString name;

    int page_size = 30;

    int current_page = 1;

    int total_pages = 0;

    static const QString api_base_url;

    QVector<QVector<QVariant>> data;

    QtCharts::QLineSeries *line_series;
    QtCharts::QChart *chart;
    QtCharts::QDateTimeAxis *axis_x;
    QtCharts::QValueAxis *axis_y;
};

#endif // NAV_HISTORY_DIALOG_H
