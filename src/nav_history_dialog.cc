#include "include/nav_history_dialog.h"

#include "./ui/ui_nav_history_dialog.h"
#include "include/networker.h"


const QString NavHistoryDialog::api_base_url = "https://danjuanapp.com/djapi/fund/nav/history";

NavHistoryDialog::NavHistoryDialog(const QString &code, const QString &name, QWidget *parent)
        : QDialog(parent), ui(new Ui::NavHistoryDialog), code(code), name(name) {
    ui->setupUi(this);

    ui->title_label->setText(tr("%1 %2 历史净值").arg(code, name));

    ui->table_widget->setColumnCount(3);
    ui->table_widget->setHorizontalHeaderLabels(
            QStringList() << tr("日期") << tr("净值") << tr("涨幅"));
    ui->table_widget->verticalHeader()->hide();

    connect(ui->more_button, &QPushButton::clicked, this, &NavHistoryDialog::get_more_history);

    get_more_history();
    ui->table_widget->resizeColumnsToContents();
    ui->table_widget->setFixedWidth(400);

    init_chart();

    this->adjustSize();
}

NavHistoryDialog::~NavHistoryDialog() {
    qDebug() << "DESTRUCTOR";
    qDebug() << line_series->detachAxis(axis_x);
    qDebug() << line_series->detachAxis(axis_y);
    chart->removeAxis(axis_x);
    chart->removeAxis(axis_y);
    chart->removeSeries(line_series);
    delete ui;
    delete line_series;
    delete chart;
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

void NavHistoryDialog::get_more_history() {
    // 已达到最大页数
    if (total_pages != 0 && current_page > total_pages) {
        return;
    }

    QVariantMap response = get_json_from_networker(
            QString("%1/%2?size=%3&page=%4")
                    .arg(api_base_url, code, QString::number(page_size), QString::number(current_page)));

    QList<QVariant> items = response["data"].toMap()["items"].toList();
    if (total_pages == 0) { // 第一次 记录总页数
        total_pages = response["data"].toMap()["total_pages"].toInt();
    }

    for (auto &item : items) {
        int row = ui->table_widget->rowCount();
        ui->table_widget->insertRow(row);
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

        data.push_back({date, nav, percentage});

        QColor color = Qt::red;
        if (percentage < 0) {
            color = Qt::darkGreen;
        }

        nav_item->setForeground(color);
        percentage_item->setForeground(color);

        ui->table_widget->setItem(row, 0, date_item);
        ui->table_widget->setItem(row, 1, nav_item);
        ui->table_widget->setItem(row, 2, percentage_item);
    }

    ++current_page;
}

void NavHistoryDialog::init_chart() {
    line_series = new QtCharts::QLineSeries();
    for (auto &item : data) {
        QString date = item[0].toString();
        double nav = item[1].toDouble();

        QDateTime moment_in_time;
        QStringList split_date = date.split(QLatin1Char('-'), Qt::SkipEmptyParts);
        moment_in_time.setDate(QDate(split_date[0].toInt(), split_date[1].toInt(), split_date[2].toInt()));
        line_series->append(qreal(moment_in_time.toMSecsSinceEpoch()), nav);

        chart = new QtCharts::QChart();
        chart->addSeries(line_series);
        chart->legend()->hide();

        axis_x = new QtCharts::QDateTimeAxis;
        axis_x->setTickCount(10);
        axis_x->setFormat("MM-dd");
        axis_x->setTitleText(tr("日期"));
        chart->addAxis(axis_x, Qt::AlignBottom);
        line_series->attachAxis(axis_x);

        axis_y = new QtCharts::QValueAxis;
        axis_y->setLabelFormat("%.4f");
        axis_y->setTitleText(tr("净值"));
        chart->addAxis(axis_y, Qt::AlignLeft);
        line_series->attachAxis(axis_y);
        axis_y->setRange(axis_y->min() - 0.5, axis_y->max() + 0.5);

        ui->chart_view->setChart(chart);
        ui->chart_view->setRenderHint(QPainter::Antialiasing);
        ui->chart_view->setMinimumWidth(600);

    }
}

