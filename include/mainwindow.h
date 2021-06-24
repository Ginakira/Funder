#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QtWidgets>

#include "include/holding_tab.h"
#include "include/networker.h"
#include "include/settings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void init();

private:
    void refresh_market_info();

    void get_stock_info(const QString &stock_code, QLabel *label);

    Ui::MainWindow *ui;
    NetWorker *networker;
    QSqlTableModel *db_model;
    HoldingTab *holding_tab;
    Settings *settings;
};

#endif // MAINWINDOW_H
