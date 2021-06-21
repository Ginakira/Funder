#ifndef HOLDINGTAB_H
#define HOLDINGTAB_H

#include <QSqlTableModel>
#include <QWidget>
#include "include/proxy_model.h"

namespace Ui {
class HoldingTab;
}

class HoldingTab : public QWidget {
Q_OBJECT

public:
    explicit HoldingTab(QWidget *parent = nullptr,
                        QSqlTableModel *db_model = nullptr);

    void refresh_records();

    ~HoldingTab() override;

signals:

    void refresh_market();

private:
    void new_record();

    void delete_record();

    Ui::HoldingTab *ui;
    QSqlTableModel *db_model;
    ProxyModel *proxy_model;
};

#endif // HOLDINGTAB_H
