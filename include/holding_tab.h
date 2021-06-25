#ifndef HOLDINGTAB_H
#define HOLDINGTAB_H

#include <QSqlTableModel>
#include <QWidget>
#include "include/proxy_model.h"
#include "include/settings.h"

namespace Ui {
class HoldingTab;
}

class HoldingTab : public QWidget {
Q_OBJECT

public:
    explicit HoldingTab(QSqlTableModel *db_model, Settings *settings, QWidget *parent = nullptr);

    void refresh_records();

    ~HoldingTab() override;

signals:

    void refresh_market();

private slots:

    void new_record();

    void delete_record();

    void edit_record();

    void calculate_summary_info();

    void save_horizontal_state();

private:
    void ui_init();


    Ui::HoldingTab *ui;
    QSqlTableModel *db_model;
    ProxyModel *proxy_model;
    Settings *settings;
};

#endif // HOLDINGTAB_H
