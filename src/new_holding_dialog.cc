#include "include/new_holding_dialog.h"

#include "./ui/ui_new_holding_dialog.h"

NewHoldingDialog::NewHoldingDialog(QWidget *parent, bool edit_mode,
                                   const QString &code,
                                   double holding_unit_cost,
                                   double holding_share, const QString &remarks)
        : QDialog(parent), ui(new Ui::NewHoldingDialog) {
    ui->setupUi(this);

    setWindowTitle(edit_mode ? tr("编辑基金信息") : tr("新增基金信息"));

    // 填充文本框
    ui->code_edit->setText(code);
    ui->holding_unit_cost_edit->setText(QString::number(holding_unit_cost));
    ui->holding_share_edit->setText(QString::number(holding_share));
    ui->remarks_edit->setText(remarks);

    // 编辑模式禁用基金代码框
    if (edit_mode) {
        ui->code_edit->setEnabled(false);
    }

    connect(this, &QDialog::accepted, this, &NewHoldingDialog::accepted_fund);
}

NewHoldingDialog::~NewHoldingDialog() { delete ui; }

void NewHoldingDialog::accepted_fund() {
    QString code = ui->code_edit->text();
    double holding_unit_cost = ui->holding_unit_cost_edit->text().toDouble();
    double holding_share = ui->holding_share_edit->text().toDouble();
    QString remarks = ui->remarks_edit->toPlainText();
    emit modified_fund(code, holding_unit_cost, holding_share, remarks);
}
