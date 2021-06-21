#include "include/mainwindow.h"

#include "./ui/ui_mainwindow.h"
#include "include/database.h"
#include "include/utility.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    networker = NetWorker::instance();

    // 初始化数据库
    if (!init_db_connect()) {
        this->setEnabled(false);
        QMessageBox::critical(this, tr("错误"), tr("数据库初始化失败"));
    }
    db_model = new QSqlTableModel(this);
    db_model->setTable("holding");
    set_db_model_header_data(db_model);
    db_model->select();
    ui->statusbar->showMessage(tr("数据库初始化成功"), 1500);

    holding_tab = new HoldingTab(this, db_model);
    ui->tab_widget->addTab(holding_tab, "持仓");

    connect(holding_tab, &HoldingTab::refresh_market, this, &MainWindow::refresh_market_info);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::init() {
    refresh_market_info();
    holding_tab->refresh_records();
}

void MainWindow::get_stock_info(const QString &stock_code, QLabel *label) {
    ui->statusbar->showMessage(tr("数据更新中...") + stock_code, 1500);

    QNetworkReply *reply =
            networker->get("https://hq.sinajs.cn/list=s_" + stock_code);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        auto *reply = qobject_cast<QNetworkReply *>(sender());
        QString str = getDecodedString(reply->readAll());

        QRegExp regex("\"(.*)\"");
        int pos = regex.indexIn(str);
        if (pos < 0) {
            qDebug() << "Regex error" << __FILE__ << __LINE__ << stock_code;
            return;
        }
        QStringList list = regex.cap(1).split(",");

        list = list.mid(0, 4);
        QString color = "red", symbol = " ↑";
        if (list[2].toDouble() < 0) {
            color = "green";
            symbol = " ↓";
        }
        if (list[1].toDouble() == 0) {
            list = list.mid(0, 2);
            list[1] = "未开盘";
        } else {
            // 保留两位
            list[1] = QString::number(list[1].toDouble(), 'f', 2);
            list[2] = QString::number(list[2].toDouble(), 'f', 2);
            // 符号
            list[1] += symbol;
            if (list[2].toDouble() > 0) {
                list[2] = "+" + list[2];
            }
            list[3] += "%";
            label->setStyleSheet("color: " + color);
        }
        label->setText(list.join("  "));
        ui->statusbar->showMessage(list[0] + tr(" 数据更新成功"), 1500);

        reply->deleteLater();
    });
}

void MainWindow::refresh_market_info() {
    get_stock_info("sh000001", ui->szzs_label);
    get_stock_info("sz399001", ui->szcz_label);
    get_stock_info("sz399006", ui->cybz_label);
    get_stock_info("sh000300", ui->hs300_label);
    get_stock_info("sh000016", ui->sz50_label);
}
