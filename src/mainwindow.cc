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
    holding_tab->refresh_market_info();
    holding_tab->refresh_records();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    settings->save_window_geometry(saveGeometry());
    event->accept();
}

void MainWindow::connect_settings_signals() {
    connect(settings_tab, &SettingsTab::row_height_changed, holding_tab, &HoldingTab::row_height_changed);
    connect(settings_tab, &SettingsTab::main_background_color_changed, holding_tab,
            &HoldingTab::main_background_color_changed);
    connect(settings_tab, &SettingsTab::secondary_background_color_changed, holding_tab,
            &HoldingTab::secondary_background_color_changed);
}
