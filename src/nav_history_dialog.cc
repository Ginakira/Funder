#include "include/nav_history_dialog.h"

#include "./ui/ui_nav_history_dialog.h"
#include "include/networker.h"

#include <QHeaderView>

const QString NavHistoryDialog::api_base_url = "https://danjuanapp.com/djapi/fund/nav/history";

NavHistoryDialog::NavHistoryDialog(const QString &code, const QString &name, Settings *settings, QWidget *parent)
        : QDialog(parent), ui(new Ui::NavHistoryDialog), code(code), name(name), settings(settings) {
    ui->setupUi(this);
    table_widget = new QTableWidget(this);
    nav_history_chart_view = new NavHistoryChartView(this);

    // 恢复窗口位置和大小
    QByteArray geometry_state = settings->load_nav_history_dialog_geometry();
    if (!geometry_state.isEmpty()) {
        restoreGeometry(geometry_state);
    }

    ui->title_label->setText(tr("%1 %2 历史净值").arg(code, name));
    init_table_widget();

    connect(ui->more_button, &QPushButton::clicked, this, [&]() { get_more_history(); });
    connect(this, &NavHistoryDialog::need_get_more_history, this, &NavHistoryDialog::get_more_history,
            Qt::QueuedConnection);

    emit need_get_more_history(6);

    ui->grid_layout->addWidget(nav_history_chart_view, 1, 0);
    ui->grid_layout->addWidget(table_widget, 1, 1);
    ui->grid_layout->setColumnStretch(1, 0);
    ui->grid_layout->setColumnStretch(0, 1);
}

NavHistoryDialog::~NavHistoryDialog() {
    delete ui;
}

QVariantMap NavHistoryDialog::get_json_from_networker(const QString &url) {
    NetWorker *networker = NetWorker::instance();
    QNetworkReply *reply = networker->get(url);

    QEventLoop synchronous;
    connect(reply, &QNetworkReply::finished, &synchronous, &QEventLoop::quit);
    synchronous.exec();

    QString content = reply->readAll();

    QJsonParseError error{};
    QJsonDocument json_doc = QJsonDocument::fromJson(content.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        qFatal("%s", error.errorString().toUtf8().constData());
    }

    QVariantMap result = json_doc.toVariant().toMap();

    reply->deleteLater();
    return result;
}

void NavHistoryDialog::get_more_history(int months) {
    // 追加到图表中的点
    QList<QPointF> points;

    for (int i = 0; i < months; ++i) {
        // 已达到最大页数
        if (total_pages != 0 && current_page > total_pages) {
            break;
        }

        QVariantMap response = get_json_from_networker(
                QString("%1/%2?size=%3&page=%4")
                        .arg(api_base_url, code, QString::number(page_size), QString::number(current_page)));

        QList<QVariant> items = response["data"].toMap()["items"].toList();
        if (total_pages == 0) { // 第一次 记录总页数
            total_pages = response["data"].toMap()["total_pages"].toInt();
        }

        for (auto &item : items) {
            int row = table_widget->rowCount();
            table_widget->insertRow(row);
            auto *date_item = new QTableWidgetItem;
            auto *nav_item = new QTableWidgetItem;
            auto *percentage_item = new QTableWidgetItem;

            QMap<QString, QVariant> item_map = item.toMap();
            QString date = item_map["date"].toString();
            double nav = item_map["nav"].toDouble();
            double percentage = item_map["percentage"].toDouble();

            date_item->setText(date.mid(5));
            nav_item->setText(QString::number(nav));
            percentage_item->setText(QString::number(percentage) + "%");

            QColor color = Qt::red;
            if (percentage < 0) {
                color = Qt::darkGreen;
            }

            nav_item->setForeground(color);
            percentage_item->setForeground(color);

            table_widget->setItem(row, 0, date_item);
            table_widget->setItem(row, 1, nav_item);
            table_widget->setItem(row, 2, percentage_item);

            // 追加图表数据
            QDateTime moment_in_time;
            QStringList split_date = date.split(QLatin1Char('-'), Qt::SkipEmptyParts);
            moment_in_time.setDate(QDate(split_date[0].toInt(), split_date[1].toInt(), split_date[2].toInt()));
            points.append({qreal(moment_in_time.toMSecsSinceEpoch()), nav});
            if (min_time.isNull()) {
                min_time = moment_in_time;
            } else {
                min_time = moment_in_time < min_time ? moment_in_time : min_time;
            }
            if (max_time.isNull()) {
                max_time = moment_in_time;
            } else {
                max_time = moment_in_time > max_time ? moment_in_time : max_time;
            }
            min_nav = fmin(min_nav, nav);
            max_nav = fmax(max_nav, nav);
        }
        ++current_page;
    }
    nav_history_chart_view->append_data_to_chart(points, min_time, max_time, min_nav, max_nav);
}

void NavHistoryDialog::init_table_widget() {
    table_widget->setColumnCount(3);
    table_widget->setHorizontalHeaderLabels(
            QStringList() << tr("日期") << tr("净值") << tr("涨幅"));
    table_widget->verticalHeader()->hide();
    table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_widget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void NavHistoryDialog::closeEvent(QCloseEvent *event) {
    settings->save_nav_history_dialog_geometry(saveGeometry());
    event->accept();
}
