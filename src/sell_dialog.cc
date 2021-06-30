#include "include/sell_dialog.h"
#include "./ui/ui_sell_dialog.h"

SellDialog::SellDialog(const QString &code, const QString &name, double total_share, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SellDialog) {
    ui->setupUi(this);

    ui->fund_info_label->setText(QString("%1-%2").arg(code, name));
    ui->tips_label->setText(QString("最多可卖出 %1 份")
                                    .arg(QString::number(total_share, 'f', 2)));

    connect(this, &QDialog::accepted, this, [=]() {
        emit sell_confirmed(ui->sell_share_edit->text().toDouble());
    });

    // 份额计算按钮
    connect(ui->all_button, &QPushButton::clicked, this, [=]() {
        ui->sell_share_edit->setText(QString::number(total_share, 'f', 2));
    });
    connect(ui->one_in_two_button, &QPushButton::clicked, this, [=]() {
        ui->sell_share_edit->setText(QString::number(total_share / 2, 'f', 2));
    });
    connect(ui->one_in_three_button, &QPushButton::clicked, this, [=]() {
        ui->sell_share_edit->setText(QString::number(total_share / 3, 'f', 2));
    });
    connect(ui->one_in_four_button, &QPushButton::clicked, this, [=]() {
        ui->sell_share_edit->setText(QString::number(total_share / 4, 'f', 2));
    });


}

SellDialog::~SellDialog() {
    delete ui;
}
