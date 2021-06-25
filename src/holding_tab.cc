#include "include/holding_tab.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>

#include "./ui/ui_holding_tab.h"
#include "include/new_holding_dialog.h"
#include "include/fund_info.h"
#include "include/database.h"
#include "include/utility.h"

HoldingTab::HoldingTab(QSqlTableModel *db_model, Settings *settings, QWidget *parent)
        : ui(new Ui::HoldingTab), db_model(db_model), settings(settings), QWidget(parent) {
    ui->setupUi(this);

    // 创建并设置代理模型
    proxy_model = new ProxyModel(this);
    proxy_model->setSourceModel(db_model);
    ui->holding_table_view->setModel(proxy_model);

    ui_init();

    connect(ui->refresh_button, &QPushButton::clicked, this,
            &HoldingTab::refresh_market);
    connect(ui->refresh_button, &QPushButton::clicked, this, &HoldingTab::refresh_records);
    connect(ui->new_button, &QPushButton::clicked, this,
            &HoldingTab::new_record);
    connect(ui->delete_button, &QPushButton::clicked, this,
            &HoldingTab::delete_record);

    // 表头列移动时存储state到设置中 以便下次启动恢复顺序
    connect(ui->holding_table_view->horizontalHeader(), &QHeaderView::sectionMoved, this,
            &HoldingTab::save_horizontal_state);
}

HoldingTab::~HoldingTab() {
    delete ui;
}

void HoldingTab::new_record() {
    NewHoldingDialog dialog(this, false);
    connect(&dialog, &NewHoldingDialog::modified_record, this,
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

void HoldingTab::delete_record() {
    QModelIndex selected_index = ui->holding_table_view->currentIndex();
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

void HoldingTab::refresh_records() {
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

    // 隐藏列
    ui->holding_table_view->setColumnHidden(ID_COL, true);  // 隐藏ID列
    ui->holding_table_view->setColumnHidden(ORDER_ID_COL, true);  // 隐藏顺序ID列
    ui->holding_table_view->setColumnHidden(HOLDING_AMOUNT_COL, true); // 隐藏持有金额
    ui->holding_table_view->setColumnHidden(VALUATION_TIME_COL, true); // 隐藏估值时间

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
    for (int i = 0; i < rows; ++i) {
        QSqlRecord record = db_model->record(i);
        total_cost += record.value(HOLDING_TOTAL_COST_KEY).toDouble();
        total_earnings += record.value(HOLDING_EARNINGS_KEY).toDouble();
        total_holding_amount += record.value(HOLDING_AMOUNT_KEY).toDouble();
        total_expected_earnings += record.value(EXPECTED_EARNINGS_KEY).toDouble();
    }
    ui->total_cost_label->setText(QString::number(total_cost, 'f', 0));
    ui->total_earnings_label->setText(QString::number(total_earnings, 'f', 2));
    ui->total_holding_amount_label->setText(QString::number(total_holding_amount, 'f', 2));
    ui->total_expected_earnings_label->setText(QString::number(total_expected_earnings, 'f', 2));

    // 设置颜色
    const QString red_color = "color: red";
    const QString green_color = "color: green";
    ui->total_cost_label->setStyleSheet(red_color);
    ui->total_earnings_label->setStyleSheet(total_earnings >= 0 ? red_color : green_color);
    ui->total_holding_amount_label->setStyleSheet(total_earnings >= 0 ? red_color : green_color);
    ui->total_expected_earnings_label->setStyleSheet(total_expected_earnings >= 0 ? red_color : green_color);
}