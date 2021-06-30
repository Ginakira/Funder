#include "include/database.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlResult>
#include <QCoreApplication>

/*
 * 持仓表字段说明
 * ---20210628-V1--
 *  0. id                    自增主键标识 INT
 *  1. order_id              显示顺序    INT NOT NULL
 *  2. code                  基金代码    NVARCHAR *
 *  3. name                  基金名称    NVARCHAR *
 *  4. holding_unit_cost     持仓成本    DOUBLE *
 *  5. holding_share         持有份额    DOUBLE *
 *  6. holding_total_cost    成本金额    DOUBLE *
 *  7. holding_earnings      持有收益    DOUBLE *
 *  8. holding_earning_rate  持有收益率  DOUBLE *
 *  9. holding_amount        持有金额    DOUBLE *
 * 10. nav                   单位净值    DOUBLE
 * 11. nav_gains             净值涨跌    DOUBLE
 * 12. nav_time              净值时间    NVARCHAR
 * 13. valuation             盘中估值    DOUBLE
 * 14. valuation_gains       估值涨跌    DOUBLE
 * 15. valuation_time        估值时间    NVARCHAR
 * 16. expected_earnings     预计收益 / 如已结算则为结算收益    DOUBLE
 * 17. remarks               备注       NVARCHAR
 * 18. settled               是否已结算  BOOLEAN
 */

bool init_db_connect() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/data.db");
    if (!db.open()) {
        QMessageBox::critical(nullptr, QObject::tr("数据库连接发生错误"),
                              db.lastError().text());
        return false;
    }

    create_table();
    if (get_holding_table_rows() > 0) {
        qDebug() << "Have exist records, before alter table" << get_holding_table_rows();
        alter_table();
        qDebug() << "After alter table";
    }

    return true;
}

void set_db_model_header_data(QSqlTableModel *model) {
    model->setHeaderData(CODE_COL, Qt::Horizontal, QObject::tr("基金代码"));
    model->setHeaderData(NAME_COL, Qt::Horizontal, QObject::tr("基金名称"));
    model->setHeaderData(HOLDING_UNIT_COST_COL, Qt::Horizontal, QObject::tr("成本价"));
    model->setHeaderData(HOLDING_SHARE_COL, Qt::Horizontal, QObject::tr("份额"));
    model->setHeaderData(HOLDING_TOTAL_COST_COL, Qt::Horizontal, QObject::tr("本金"));
    model->setHeaderData(HOLDING_EARNINGS_COL, Qt::Horizontal, QObject::tr("盈亏"));
    model->setHeaderData(HOLDING_EARNING_RATE_COL, Qt::Horizontal, QObject::tr("收益率"));
    model->setHeaderData(HOLDING_AMOUNT_COL, Qt::Horizontal, QObject::tr("持有金额"));
    model->setHeaderData(NAV_COL, Qt::Horizontal, QObject::tr("净值"));
    model->setHeaderData(NAV_GAINS_COL, Qt::Horizontal, QObject::tr("收盘"));
    model->setHeaderData(NAV_TIME_COL, Qt::Horizontal, QObject::tr("净值时间"));
    model->setHeaderData(VALUATION_COL, Qt::Horizontal, QObject::tr("估值"));
    model->setHeaderData(VALUATION_GAINS_COL, Qt::Horizontal, QObject::tr("涨幅"));
    model->setHeaderData(VALUATION_TIME_COL, Qt::Horizontal, QObject::tr("估值时间"));
    model->setHeaderData(EXPECTED_EARNINGS_COL, Qt::Horizontal, QObject::tr("预计收益"));
    model->setHeaderData(REMARKS_COL, Qt::Horizontal, QObject::tr("备注"));
}

int get_holding_table_rows() {
    QSqlQuery query;
    query.exec("SELECT COUNT(id) FROM holding;");
    int rows = 0;
    while (query.next()) {
        rows = query.value(0).toInt();
    }
    return rows;
}

bool create_table() {
    QSqlQuery query;
    QString create_table_command =
            "CREATE TABLE IF NOT EXISTS holding ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "order_id INTEGER NOT NULL,"
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
            "remarks NVARCHAR,"
            "settled BOOLEAN DEFAULT false)";

    if (!query.exec(create_table_command)) {
        qDebug() << query.lastError();
        return false;
    }
    return true;
}

bool alter_table() {
    // 丑陋的实现 待优化 通过忽略错误添加不存在的列
    QSqlQuery query;
    std::vector<QString> commands = {
            "ALTER TABLE holding ADD COLUMN order_id INTEGER NOT NULL;",
            "ALTER TABLE holding ADD COLUMN code NVARCHAR NOT NULL;",
            "ALTER TABLE holding ADD COLUMN name NVARCHAR NOT NULL;",
            "ALTER TABLE holding ADD COLUMN holding_unit_cost DOUBLE NOT NULL;",
            "ALTER TABLE holding ADD COLUMN holding_share DOUBLE NOT NULL;",
            "ALTER TABLE holding ADD COLUMN holding_total_cost DOUBLE NOT NULL;",
            "ALTER TABLE holding ADD COLUMN holding_earnings DOUBLE NOT NULL;",
            "ALTER TABLE holding ADD COLUMN holding_earning_rate DOUBLE NOT NULL;",
            "ALTER TABLE holding ADD COLUMN holding_amount DOUBLE NOT NULL;",
            "ALTER TABLE holding ADD COLUMN nav DOUBLE;",
            "ALTER TABLE holding ADD COLUMN nav_gains DOUBLE;",
            "ALTER TABLE holding ADD COLUMN nav_time NVARCHAR;",
            "ALTER TABLE holding ADD COLUMN valuation DOUBLE;",
            "ALTER TABLE holding ADD COLUMN valuation_gains DOUBLE;",
            "ALTER TABLE holding ADD COLUMN valuation_time NVARCHAR;",
            "ALTER TABLE holding ADD COLUMN expected_earnings DOUBLE;",
            "ALTER TABLE holding ADD COLUMN remarks NVARCHAR;",
            "ALTER TABLE holding ADD COLUMN settled BOOLEAN DEFAULT false;"
    };

    int count = 0;
    for (const auto &command : commands) {
        query.exec(command);
        if (query.lastError().type() == QSqlError::NoError) {
            ++count;
        }
    }
    qDebug() << "Altered" << count << "columns.";
    return true;
}