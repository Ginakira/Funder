#ifndef HOLDINGTAB_H
#define HOLDINGTAB_H

#include <QSqlTableModel>
#include <QWidget>
#include <QAction>
#include <QMenu>
#include <QLabel>

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

    void refresh_market_info();

    ~HoldingTab() override;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

public slots:

    void row_height_changed(int height);

private slots:

    void new_fund();

    void delete_fund();

    void edit_fund();

    void buy_fund();

    void sell_fund();

    void calculate_summary_info();

    void save_horizontal_state();

    void context_menu_slot(const QPoint &pos);

private:
    void ui_init();

    void init_context_menu(); // 初始化右键菜单相关设置

    void get_stock_info(const QString &stock_code, QLabel *label);

    QMenu *context_menu;
    QAction *buy_action;
    QAction *sell_action;
    QAction *nav_history_action;
    QAction *edit_action;
    QAction *delete_action;

    Ui::HoldingTab *ui;
    QSqlTableModel *db_model;
    ProxyModel *proxy_model;
    Settings *settings;
};

#endif // HOLDINGTAB_H
