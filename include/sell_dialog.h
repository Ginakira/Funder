#ifndef SELL_DIALOG_H
#define SELL_DIALOG_H

#include <QDialog>

namespace Ui {
class SellDialog;
}

class SellDialog : public QDialog {
Q_OBJECT

public:
    explicit SellDialog(const QString &code, const QString &name, double total_share, QWidget *parent = nullptr);

    ~SellDialog() override;

signals:

    void sell_confirmed(double sell_share);


private:
    Ui::SellDialog *ui;
};

#endif // SELL_DIALOG_H
