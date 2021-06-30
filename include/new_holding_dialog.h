#ifndef NEWHOLDINGDIALOG_H
#define NEWHOLDINGDIALOG_H

#include <QDialog>

namespace Ui {
class NewHoldingDialog;
}

class NewHoldingDialog : public QDialog {
Q_OBJECT

public:
    explicit NewHoldingDialog(QWidget *parent = nullptr, bool edit_mode = false,
                              const QString &code = "",
                              double holding_unit_cost = 0,
                              double holding_share = 0,
                              const QString &remarks = "");

    ~NewHoldingDialog() override;

signals:

    void modified_fund(const QString &code, double holding_unit_cost,
                       double holding_share, const QString &remarks);

private:
    void accepted_fund();

    Ui::NewHoldingDialog *ui;
};

#endif // NEWHOLDINGDIALOG_H
