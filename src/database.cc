#include "include/database.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QCoreApplication>

bool init_db_connect() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/data.db");
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
    model->setHeaderData(CODE_COL, Qt::Horizontal, QObject::tr("基金代码"));
    model->setHeaderData(NAME_COL, Qt::Horizontal, QObject::tr("基金名称"));
    model->setHeaderData(HOLDING_UNIT_COST_COL, Qt::Horizontal, QObject::tr("持仓成本"));
    model->setHeaderData(HOLDING_SHARE_COL, Qt::Horizontal, QObject::tr("持有份额"));
    model->setHeaderData(HOLDING_TOTAL_COST_COL, Qt::Horizontal, QObject::tr("成本金额"));
    model->setHeaderData(HOLDING_EARNINGS_COL, Qt::Horizontal, QObject::tr("持有收益"));
    model->setHeaderData(HOLDING_EARNING_RATE_COL, Qt::Horizontal, QObject::tr("收益率"));
    model->setHeaderData(HOLDING_AMOUNT_COL, Qt::Horizontal, QObject::tr("持有金额"));
    model->setHeaderData(NAV_COL, Qt::Horizontal, QObject::tr("单位净值"));
    model->setHeaderData(NAV_GAINS_COL, Qt::Horizontal, QObject::tr("净值涨跌"));
    model->setHeaderData(NAV_TIME_COL, Qt::Horizontal, QObject::tr("净值时间"));
    model->setHeaderData(VALUATION_COL, Qt::Horizontal, QObject::tr("盘中估值"));
    model->setHeaderData(VALUATION_GAINS_COL, Qt::Horizontal, QObject::tr("估值涨跌"));
    model->setHeaderData(VALUATION_TIME_COL, Qt::Horizontal, QObject::tr("估值时间"));
    model->setHeaderData(EXPECTED_EARNINGS_COL, Qt::Horizontal, QObject::tr("预计收益"));
    model->setHeaderData(REMARKS_COL, Qt::Horizontal, QObject::tr("备注"));
}
