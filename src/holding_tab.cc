#include "include/holding_tab.h"

#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>
#include <QDate>
#include <QMouseEvent>
#include <QNetworkReply>
#include <include/networker.h>

#include "./ui/ui_holding_tab.h"
#include "include/new_holding_dialog.h"
#include "include/fund_info.h"
#include "include/database.h"
#include "include/utility.h"
#include "include/buy_dialog.h"
#include "include/sell_dialog.h"
#include "include/nav_history_dialog.h"

HoldingTab::HoldingTab(QSqlTableModel *db_model, Settings *settings, QWidget *parent)
        : ui(new Ui::HoldingTab), db_model(db_model), settings(settings), QWidget(parent) {
    ui->setupUi(this);

    // 给此选项卡安装事件过滤器 以单击空白处取消选择
    this->installEventFilter(this);

    // 创建并设置代理模型
    proxy_model = new ProxyModel(this);
    proxy_model->setSourceModel(db_model);
    ui->holding_table_view->setModel(proxy_model);

    ui_init();

    // 功能按钮信号槽绑定
    connect(ui->refresh_button, &QPushButton::clicked, this,
            &HoldingTab::refresh_market_info);
    connect(ui->refresh_button, &QPushButton::clicked, this, &HoldingTab::refresh_records);
    connect(ui->new_button, &QPushButton::clicked, this,
            &HoldingTab::new_fund);

    // 表头列移动、改变大小时存储state到设置中 以便下次启动恢复顺序
    connect(ui->holding_table_view->horizontalHeader(), &QHeaderView::sectionMoved, this,
            &HoldingTab::save_horizontal_state);
    connect(ui->holding_table_view->horizontalHeader(), &QHeaderView::sectionResized, this,
            &HoldingTab::save_horizontal_state);

    // 初始化右键菜单相关设置
    init_context_menu();
}

HoldingTab::~HoldingTab() {
    delete ui;
}

void HoldingTab::new_fund() {
    NewHoldingDialog dialog(this, false);
    connect(&dialog, &NewHoldingDialog::modified_fund, this,
            [=](const QString &code, double holding_unit_cost,
                double holding_share, const QString &remarks) {
                FundInfo fund(db_model);
                fund.set_code(code);
                fund.set_holding_unit_cost(holding_unit_cost);
                fund.set_holding_share(holding_share);
                fund.set_remarks(remarks);
                fund.insert_new_record_to_database();
                ui->holding_table_view->scrollToBottom();

                calculate_summary_info();
            });
    dialog.exec();
}

void HoldingTab::delete_fund() {
    QModelIndex selected_index = ui->holding_table_view->currentIndex();
    if (!selected_index.isValid()) {
        return;
    }

    QModelIndex deleted_order_index = db_model->index(selected_index.row(), ORDER_ID_COL);
    int deleted_order = db_model->data(deleted_order_index).toInt();

    if (QMessageBox::question(this, tr("删除"),
                              tr("您确定要删除这条记录吗？"))
        & (QMessageBox::No | QMessageBox::Escape)) {
        return;
    }

    if (!db_model->removeRow(selected_index.row())) {
        QMessageBox::critical(this, tr("删除失败"),
                              db_model->lastError().text());
        qDebug() << db_model->lastError();
        return;
    }
    // 修改order_id
    for (int i = 0; i < db_model->rowCount(); ++i) {
        QModelIndex order_index = db_model->index(i, ORDER_ID_COL);
        int origin_order = db_model->data(order_index).toInt();
        if (origin_order > deleted_order) {
            db_model->setData(order_index, origin_order - 1);
            db_model->submit();
        }
    }
    db_model->select();

    calculate_summary_info();
}


void HoldingTab::edit_fund() {
    QModelIndex selected_index = ui->holding_table_view->currentIndex();
    if (!selected_index.isValid()) {
        return;
    }
    FundInfo fund(db_model, db_model->record(selected_index.row()));

    NewHoldingDialog dialog(this, true,
                            fund.get_code(),
                            fund.get_holding_unit_cost(),
                            fund.get_holding_share(),
                            fund.get_remarks());
    connect(&dialog, &NewHoldingDialog::modified_fund, this,
            [&](const QString &code, double holding_unit_cost,
                double holding_share, const QString &remarks) {

                fund.set_code(code);
                fund.set_holding_unit_cost(holding_unit_cost);
                fund.set_holding_share(holding_share);
                fund.set_remarks(remarks);
                fund.refresh_and_save_record_changes_to_database(selected_index.row());

                calculate_summary_info();
            });
    dialog.exec();
}


void HoldingTab::refresh_records() {
    // 取消选中
    ui->holding_table_view->clearSelection();

    int rows = db_model->rowCount();
    if (rows == 0) {
        return;
    }

    ui->update_label->show();
    ui->update_progress_bar->show();
    ui->update_progress_bar->setValue(0);

    int per_percent = 100 / rows;
    for (int i = 0; i < rows; ++i) {
        FundInfo fund(db_model, db_model->record(i));
        ui->update_label->setText(tr("正在更新%1...").arg(fund.get_name()));
        fund.refresh_and_save_record_changes_to_database(i);
        ui->update_progress_bar->setValue((i + 1) * per_percent);
    }

    ui->update_label->hide();
    ui->update_progress_bar->hide();

    calculate_summary_info();
}

void HoldingTab::ui_init() {
    // 隐藏基金信息更新进度条
    ui->update_label->hide();
    ui->update_progress_bar->hide();

    ui->holding_table_view->setSelectionMode(
            QAbstractItemView::SingleSelection);
    ui->holding_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->holding_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 设置根据Order_id列排序
    ui->holding_table_view->sortByColumn(ORDER_ID_COL, Qt::AscendingOrder);

    // 隐藏汇总信息中的已结算收益
    ui->settled_earnings_text_label->hide();
    ui->settled_earnings_label->hide();

    // 设置列可移动位置
    ui->holding_table_view->horizontalHeader()->setSectionsMovable(true);

    // 设置行可移动顺序
    ui->holding_table_view->setDragEnabled(true);
    ui->holding_table_view->setDragDropMode(QAbstractItemView::InternalMove);
    ui->holding_table_view->setDefaultDropAction(Qt::MoveAction);
    // 从设置中恢复列（表头）状态
    QByteArray horizontal_state = settings->load_horizontal_header_state_to_settings();
    if (!horizontal_state.isEmpty()) {
        ui->holding_table_view->horizontalHeader()->restoreState(horizontal_state);
    }

    // 隐藏列
    ui->holding_table_view->setColumnHidden(ID_COL, true);  // 隐藏ID列
    ui->holding_table_view->setColumnHidden(ORDER_ID_COL, true);  // 隐藏顺序ID列
    ui->holding_table_view->setColumnHidden(HOLDING_AMOUNT_COL, true); // 隐藏持有金额
    ui->holding_table_view->setColumnHidden(VALUATION_TIME_COL, true); // 隐藏估值时间
    ui->holding_table_view->setColumnHidden(SETTLED_COL, true); // 隐藏是否已结算标记
    ui->holding_table_view->setColumnHidden(NAV_TIME_COL, true); // 隐藏净值时间

    // 设置表格颜色交替
    ui->holding_table_view->setAlternatingRowColors(true);

    // 恢复表格行高
    ui->holding_table_view->verticalHeader()->setDefaultSectionSize(settings->load_row_height());
}

void HoldingTab::save_horizontal_state() {
    QByteArray state = ui->holding_table_view->horizontalHeader()->saveState();
    settings->save_horizontal_header_state_to_settings(state);
}

// 计算按钮上方的汇总信息
void HoldingTab::calculate_summary_info() {
    int rows = db_model->rowCount();
    double total_cost = 0; // 成本总金额
    double total_earnings = 0; // 持有总收益
    double total_holding_amount = 0; // 持有总金额
    double total_expected_earnings = 0; // 预期未结算总收益
    double total_settled_earnings = 0; // 已结算总收益
    bool total_settled = false; // 是否有已结算的基金

    ui->settled_earnings_text_label->hide();
    ui->settled_earnings_label->hide();

    for (int i = 0; i < rows; ++i) {
        QSqlRecord record = db_model->record(i);
        total_cost += record.value(HOLDING_TOTAL_COST_KEY).toDouble();
        total_earnings += record.value(HOLDING_EARNINGS_KEY).toDouble();
        total_holding_amount += record.value(HOLDING_AMOUNT_KEY).toDouble();

        // 判断是否已结算
        if (record.value(SETTLED_KEY).toBool()) {
            total_settled = true;
            total_settled_earnings += record.value(EXPECTED_EARNINGS_COL).toDouble();
        } else {
            total_expected_earnings += record.value(EXPECTED_EARNINGS_COL).toDouble();
        }
    }
    ui->total_cost_label->setText(QString::number(total_cost, 'f', 0));
    ui->total_earnings_label->setText(QString::number(total_earnings, 'f', 2));
    ui->total_holding_amount_label->setText(QString::number(total_holding_amount, 'f', 2));
    ui->total_expected_earnings_label->setText(QString::number(total_expected_earnings, 'f', 2));
    // 已结算收益
    ui->settled_earnings_label->setHidden(!total_settled);
    ui->settled_earnings_text_label->setHidden(!total_settled);
    ui->settled_earnings_label->setText(QString::number(total_settled_earnings, 'f', 2));

    // 设置颜色
    const QString red_color = "color: red";
    const QString green_color = "color: green";
    ui->total_cost_label->setStyleSheet(red_color);
    ui->total_earnings_label->setStyleSheet(total_earnings >= 0 ? red_color : green_color);
    ui->total_holding_amount_label->setStyleSheet(total_earnings >= 0 ? red_color : green_color);
    ui->total_expected_earnings_label->setStyleSheet(total_expected_earnings >= 0 ? red_color : green_color);
    ui->settled_earnings_label->setStyleSheet(total_settled_earnings >= 0 ? red_color : green_color);
}

void HoldingTab::context_menu_slot(const QPoint &pos) {
    QModelIndex index = ui->holding_table_view->indexAt(pos); //获取鼠标点击位置项的索引
    if (index.isValid()) { //数据项是否有效，空白处点击无菜单
        context_menu->exec(QCursor::pos());//数据项有效才显示菜单
    } else {
        ui->holding_table_view->clearSelection();
    }
}

void HoldingTab::init_context_menu() {
    ui->holding_table_view->setContextMenuPolicy(Qt::CustomContextMenu);

    context_menu = new QMenu(this);
    buy_action = new QAction(tr("加仓"), this);
    sell_action = new QAction(tr("减仓"), this);
    nav_history_action = new QAction(tr("历史净值"), this);
    edit_action = new QAction(tr("编辑"), this);
    delete_action = new QAction(tr("删除"), this);
    context_menu->addAction(buy_action);
    context_menu->addAction(sell_action);
    context_menu->addAction(nav_history_action);
    context_menu->addAction(edit_action);
    context_menu->addAction(delete_action);

    connect(ui->holding_table_view,
            &QTableView::customContextMenuRequested,
            this,
            &HoldingTab::context_menu_slot);

    connect(buy_action, &QAction::triggered, this, &HoldingTab::buy_fund);
    connect(sell_action, &QAction::triggered, this, &HoldingTab::sell_fund);
    connect(nav_history_action, &QAction::triggered, this, &HoldingTab::show_nav_history);
    connect(edit_action, &QAction::triggered, this, &HoldingTab::edit_fund);
    connect(delete_action, &QAction::triggered, this, &HoldingTab::delete_fund);
}

void HoldingTab::buy_fund() {
    QModelIndex index = ui->holding_table_view->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QSqlRecord record = db_model->record(index.row());
    FundInfo fund(db_model, record);
    // 先刷新信息 防止在未更新到结算状态之前加仓
    fund.refresh_and_save_record_changes_to_database(index.row());

    // 从设置中读取保存的买入手续费
    double stored_service_charge = settings->load_buy_service_charge(fund.get_code());

    BuyDialog dialog(fund.get_code(),
                     fund.get_name(),
                     fund.get_nav(),
                     stored_service_charge,
                     this);

    connect(&dialog, &BuyDialog::buy_confirmed, this,
            [=, &fund](double buy_amount, double buy_nav, double service_charge) {
                // 存储输入的手续费费率
                settings->save_buy_service_charge(fund.get_code(), service_charge);

                double charges = buy_amount * (service_charge / 100); // 手续费具体金额
                buy_amount -= charges;
                double buy_share = buy_amount / buy_nav; // 买入的份额

                double holding_share = fund.get_holding_share() + buy_share; // 加仓后总份额
                double holding_amount = fund.get_holding_total_cost() + buy_amount; // 加仓后总持仓金额
                double holding_unit_cost = holding_amount / holding_share;
                fund.set_holding_share(holding_share);
                fund.set_holding_amount(holding_amount);
                fund.set_holding_unit_cost(holding_unit_cost);
                fund.refresh_and_save_record_changes_to_database(index.row());

                calculate_summary_info();
            });
    dialog.exec();
}

void HoldingTab::sell_fund() {
    QModelIndex index = ui->holding_table_view->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QSqlRecord record = db_model->record(index.row());
    FundInfo fund(db_model, record);
    fund.refresh_and_save_record_changes_to_database(index.row());

    SellDialog dialog(fund.get_code(), fund.get_name(), fund.get_holding_share(), this);

    connect(&dialog, &SellDialog::sell_confirmed, this, [=, &fund](double sell_share) {
        double holding_share = fund.get_holding_share() - sell_share; // 减仓后总份额
        double holding_amount = fund.get_holding_total_cost() - (fund.get_nav() * sell_share); // 减仓后总持仓金额
        double holding_unit_cost = holding_amount / holding_share;
        fund.set_holding_share(holding_share);
        fund.set_holding_amount(holding_amount);
        fund.set_holding_unit_cost(holding_unit_cost);
        fund.refresh_and_save_record_changes_to_database(index.row());

        calculate_summary_info();
    });
    dialog.exec();
}

bool HoldingTab::eventFilter(QObject *obj, QEvent *event) {
    if (obj == this) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto *mouse_event = dynamic_cast<QMouseEvent *>(event);
            QModelIndex index = ui->holding_table_view->indexAt(mouse_event->pos());
            if (!index.isValid()) {
                ui->holding_table_view->clearSelection();
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void HoldingTab::row_height_changed(int height) {
    ui->holding_table_view->verticalHeader()->setDefaultSectionSize(height);
    settings->save_row_height(height);
}

void HoldingTab::get_stock_info(const QString &stock_code, QLabel *label) {
    NetWorker *networker = NetWorker::instance();
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

void HoldingTab::refresh_market_info() {
    get_stock_info("sh000001", ui->szzs_label);
    get_stock_info("sz399001", ui->szcz_label);
    get_stock_info("sz399006", ui->cybz_label);
    get_stock_info("sh000300", ui->hs300_label);
    get_stock_info("sh000016", ui->sz50_label);
}

void HoldingTab::show_nav_history() {
    QModelIndex index = ui->holding_table_view->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QSqlRecord record = db_model->record(index.row());
    NavHistoryDialog dialog(record.value(CODE_COL).toString(), record.value(NAME_COL).toString(), this);
    dialog.exec();
}
