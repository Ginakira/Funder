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

private slots:

    void new_record();

    void delete_record();

    void calculate_summary_info();

    void save_horizontal_state();

private:
    void ui_init();


    Ui::HoldingTab *ui;
    QSqlTableModel *db_model;
    ProxyModel *proxy_model;
};

#endif // HOLDINGTAB_H
