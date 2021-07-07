#include "include/mainwindow.h"

#include "./ui/ui_mainwindow.h"
#include "include/database.h"
#include "include/utility.h"
#include "include/my_sql_table_model.h"

#include <QFont>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    networker = NetWorker::instance();

    // 初始化数据库
    if (!init_db_connect()) {
        this->setEnabled(false);
        QMessageBox::critical(this, tr("错误"), tr("数据库初始化失败"));
    }

    db_model = new MySqlTableModel(this);
    db_model->setTable("holding");
    set_db_model_header_data(db_model);
    db_model->select();

    // 初始化设置信息
    settings = new Settings(QCoreApplication::applicationDirPath() + "/settings.ini");

    // 加仓选项卡
    holding_tab = new HoldingTab(db_model, settings, this);
    ui->tab_widget->addTab(holding_tab, tr("持仓"));
    // 设置选项卡
    settings_tab = new SettingsTab(settings, this);
    ui->tab_widget->addTab(settings_tab, tr("设置"));

    connect(holding_tab, &HoldingTab::refresh_market, this, &MainWindow::refresh_market_info);

    // 恢复窗口位置和大小
    QByteArray geometry_state = settings->load_window_geometry();
    if (!geometry_state.isEmpty()) {
        restoreGeometry(geometry_state);
    }

    connect_settings_signals();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::init() {
    refresh_market_info();
    holding_tab->refresh_records();
}

void MainWindow::get_stock_info(const QString &stock_code, QLabel *label) {
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

void MainWindow::closeEvent(QCloseEvent *event) {
    settings->save_window_geometry(saveGeometry());
    event->accept();
}

void MainWindow::connect_settings_signals() {
    connect(settings_tab, &SettingsTab::row_height_changed, holding_tab, &HoldingTab::row_height_changed);
}
