#include "include/database.h"

#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

bool init_db_connect() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data.db");
    if (!db.open()) {
        QMessageBox::critical(nullptr, QObject::tr("数据库连接发生错误"),
                              db.lastError().text());
        return false;
    }

    QSqlQuery query;
    /*
     * 持仓表字段说明
     * ---20210620---
     *  0. id                    自增主键标识 INT *
     *  1. code                  基金代码    NVARCHAR *
     *  2. name                  基金名称    NVARCHAR *
     *  3. holding_unit_cost     持仓成本    DOUBLE *
     *  4. holding_share         持有份额    DOUBLE *
     *  5. holding_total_cost    成本金额    DOUBLE *
     *  6. holding_earnings      持有收益    DOUBLE *
     *  7. holding_earning_rate  持有收益率  DOUBLE *
     *  8. holding_amount        持有金额    DOUBLE *
     *  9. nav                   单位净值    DOUBLE
     * 10. nav_gains             净值涨跌    DOUBLE
     * 11. nav_time              净值时间    NVARCHAR
     * 12. valuation             盘中估值    DOUBLE
     * 13. valuation_gains       估值涨跌    DOUBLE
     * 14. valuation_time        估值时间    NVARCHAR
     * 15. expected_earnings     预计收益    DOUBLE
     * 16. remarks               备注       NVARCHAR
     */
    QString create_table_command =
            "CREATE TABLE IF NOT EXISTS holding ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "code NVARCHAR NOT NULL,"
            "name NVARCHAR NOT NULL,"
            "holding_unit_cost DOUBLE NOT NULL,"
            "holding_share DOUBLE NOT NULL,"
            "holding_total_cost DOUBLE NOT NULL,"
            "holding_earnings DOUBLE NOT NULL,"
            "holding_earning_rate DOUBLE NOT NULL,"
            "holding_amount DOUBLE NOT NULL,"
            "nav DOUBLE,"
            "nav_gains DOUBLE,"
            "nav_time NVARCHAR,"
            "valuation DOUBLE,"
            "valuation_gains DOUBLE,"
            "valuation_time NVARCHAR,"
            "expected_earnings DOUBLE,"
            "remarks NVARCHAR)";

    if (!query.exec(create_table_command)) {
        qDebug() << query.lastError();
    }

    return true;
}

void set_db_model_header_data(QSqlTableModel *model) {
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("基金代码"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("基金名称"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("持仓成本"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("持有份额"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("成本金额"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("持有收益"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("收益率"));
    model->setHeaderData(8, Qt::Horizontal, QObject::tr("持有金额"));
    model->setHeaderData(9, Qt::Horizontal, QObject::tr("单位净值"));
    model->setHeaderData(10, Qt::Horizontal, QObject::tr("净值涨跌"));
    model->setHeaderData(11, Qt::Horizontal, QObject::tr("净值时间"));
    model->setHeaderData(12, Qt::Horizontal, QObject::tr("盘中估值"));
    model->setHeaderData(13, Qt::Horizontal, QObject::tr("估值涨跌"));
    model->setHeaderData(14, Qt::Horizontal, QObject::tr("估值时间"));
    model->setHeaderData(15, Qt::Horizontal, QObject::tr("预计收益"));
    model->setHeaderData(16, Qt::Horizontal, QObject::tr("备注"));
}
