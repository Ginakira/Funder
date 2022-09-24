#ifndef NAV_HISTORY_DIALOG_H
#define NAV_HISTORY_DIALOG_H

#include <QDialog>
#include <limits>
#include <QDateTime>
#include <QTableWidget>
#include <QtNetwork>

#include "include/nav_history_chart_view.h"
#include "include/settings.h"

namespace Ui {
class NavHistoryDialog;
}

class NavHistoryDialog : public QDialog {
Q_OBJECT

public:
    explicit NavHistoryDialog(const QString &code, const QString &name, Settings *settings,
                              QWidget *parent = nullptr);

    ~NavHistoryDialog() override;

signals:

    void need_get_more_history(int months = 1);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:

    void get_more_history(int months = 1);

private:
    static QVariantMap
    get_json_from_networker(const QString &url, const QString &referer_url = QString(),
                            const std::function<void(QNetworkRequest *)> &request_modifier = nullptr);

    void init_table_widget();

    Ui::NavHistoryDialog *ui;
    const QString code;
    const QString name;

    int page_size = 30;
    int current_page = 1;
    int total_pages = 0;
    static const QString api_base_url;

    QTableWidget *table_widget;
    NavHistoryChartView *nav_history_chart_view;

    QDateTime min_time;
    QDateTime max_time;
    double min_nav = std::numeric_limits<double>::max();
    double max_nav = 0;

    Settings *settings;
};

#endif // NAV_HISTORY_DIALOG_H
