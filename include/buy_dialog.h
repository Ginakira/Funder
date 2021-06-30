#ifndef BUY_DIALOG_H
#define BUY_DIALOG_H

#include <QDialog>

namespace Ui {
class BuyDialog;
}

class BuyDialog : public QDialog {
Q_OBJECT

public:
    explicit BuyDialog(const QString &code,
                       const QString &name,
                       double nav,
                       double service_charge,
                       QWidget *parent = nullptr);

    ~BuyDialog() override;

signals:

    void buy_confirmed(double buy_amount, double buy_nav, double service_charge);

private:
    Ui::BuyDialog *ui;
};

#endif // BUY_DIALOG_H
