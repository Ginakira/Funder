#include "include/holding_tab.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>

#include "./ui/ui_holding_tab.h"
#include "include/new_holding_dialog.h"
#include "include/fund_info.h"

HoldingTab::HoldingTab(QWidget *parent, QSqlTableModel *db_model)
        : QWidget(parent), ui(new Ui::HoldingTab), db_model(db_model) {
    ui->setupUi(this);

    // 隐藏基金信息更新进度条
    ui->update_label->hide();
    ui->update_progress_bar->hide();

    proxy_model = new ProxyModel(this);
    proxy_model->setSourceModel(db_model);

    ui->holding_table_view->setModel(proxy_model);
    ui->holding_table_view->setSelectionMode(
            QAbstractItemView::SingleSelection);
    ui->holding_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->holding_table_view->resizeColumnsToContents();
    ui->holding_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 隐藏列
    ui->holding_table_view->setColumnHidden(0, true);  // 隐藏ID列
    ui->holding_table_view->setColumnHidden(8, true); // 隐藏持有金额
    ui->holding_table_view->setColumnHidden(14, true); // 隐藏估值时间

    // 设置列可移动位置
    ui->holding_table_view->horizontalHeader()->setSectionsMovable(true);

    connect(ui->refresh_button, &QPushButton::clicked, this,
            &HoldingTab::refresh_market);
    connect(ui->refresh_button, &QPushButton::clicked, this, &HoldingTab::refresh_records);
    connect(ui->new_button, &QPushButton::clicked, this,
            &HoldingTab::new_record);
    connect(ui->delete_button, &QPushButton::clicked, this,
            &HoldingTab::delete_record);
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
                ui->holding_table_view->resizeColumnsToContents();
            });
    dialog.exec();
}

void HoldingTab::delete_record() {
    QModelIndex selected_index = ui->holding_table_view->currentIndex();
    if (QMessageBox::question(this, tr("删除"),
                              tr("您确定要删除这条记录吗？"))
        & (QMessageBox::No | QMessageBox::Escape)) {
        return;
    }

    if (!db_model->removeRow(selected_index.row())) {
        QMessageBox::critical(this, tr("删除失败"),
                              db_model->lastError().text());
        qDebug() << db_model->lastError();
    }
    db_model->select();
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
        fund.refresh_and_save_record_changes_to_database(i);
        ui->update_progress_bar->setValue((i + 1) * per_percent);
    }

    ui->holding_table_view->resizeColumnsToContents();

    ui->update_label->hide();
    ui->update_progress_bar->hide();
}
