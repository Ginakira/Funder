#include "include/buy_dialog.h"
#include "./ui/ui_buy_dialog.h"

BuyDialog::BuyDialog(const QString &code,
                     const QString &name,
                     double nav,
                     double service_charge,
                     QWidget *parent) : QDialog(parent),
                                        ui(new Ui::BuyDialog) {
    ui->setupUi(this);

    ui->fund_info_label->setText(QString("%1-%2")
                                         .arg(code, name));
    if (nav != 0) {
        ui->buy_nav_edit->setText(QString::number(nav));
    }

    if (service_charge != 0) {
        ui->service_charge_edit->setText(QString::number(service_charge));
    }

    connect(this, &QDialog::accepted, this, [=]() {
        double buy_amount = ui->buy_amount_edit->text().toDouble();
        double buy_nav = ui->buy_nav_edit->text().toDouble();
        double service_charge = ui->service_charge_edit->text().toDouble();
        emit buy_confirmed(buy_amount, buy_nav, service_charge);
    });
}

BuyDialog::~BuyDialog() {
    delete ui;
}
