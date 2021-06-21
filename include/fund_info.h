#ifndef FUNDINFO_H
#define FUNDINFO_H

#include <QObject>
#include <QVariantMap>
#include <QSqlTableModel>

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

class FundInfo : public QObject {
Q_OBJECT
public:
    explicit FundInfo(QSqlTableModel *model, QObject *parent = nullptr);

    explicit FundInfo(QSqlTableModel *model, const QSqlRecord &record, QObject *parent = nullptr);

    ~FundInfo() override = default;

    // GETTERS AND SETTERS
    const QString &get_code() const;

    void set_code(const QString &code);

    const QString &get_name() const;

    void set_name(const QString &name);

    double get_holding_unit_cost() const;

    void set_holding_unit_cost(double holding_unit_cost);

    double get_holding_share() const;

    void set_holding_share(double holding_share);

    double get_holding_total_cost() const;

    void set_holding_total_cost(double holding_total_cost);

    double get_holding_earnings() const;

    void set_holding_earnings(double holding_earnings);

    double get_holding_earning_rate() const;

    void set_holding_earning_rate(double holding_earning_rate);

    double get_holding_amount() const;

    void set_holding_amount(double holding_amount);

    double get_nav() const;

    void set_nav(double nav);

    const QString &get_nav_time() const;

    void set_nav_time(const QString &nav_time);

    double get_nav_gains() const;

    void set_nav_gains(double nav_gains);

    double get_valuation() const;

    void set_valuation(double valuation);

    double get_valuation_gains() const;

    void set_valuation_gains(double valuation_gains);

    QString get_valuation_time() const;

    void set_valuation_time(const QString &valuation_time);

    double get_expected_earnings() const;

    void set_expected_earnings(double expected_earnings);

    const QString &get_remarks() const;

    void set_remarks(const QString &remarks);
    // GETTERS AND SETTERS END

    bool insert_new_record_to_database();

    bool refresh_and_save_record_changes_to_database(int row);

private:
    static QVariantMap
    get_json_from_networker(const QString &url, const std::function<QString(const QString &)> &filter = nullptr);

    bool refresh_fund_info();

    bool calculate_other_info();

    bool get_name_and_valuation_info(); // 获取名称和估值信息（天天基金）

    bool get_nav_info(); // 获取净值信息（蛋卷）

    QSqlRecord save_to_record(); // 返回新的Record

    bool save_to_record(QSqlRecord &record); // 使用成员值设置传入的Record

    void set_info_from_record(const QSqlRecord &record);


private:
    QSqlTableModel *model;
    QString code = "";                // 基金代码（用户设定）
    QString name = "";                // 基金名称（接口获取）
    double holding_unit_cost = 0;     // 持仓成本（用户设定/设定后计算）
    double holding_share = 0;         // 持有份额（用户设定/设定后计算）
    double holding_total_cost = 0;    // 成本金额（设定后计算）
    double holding_earnings = 0;      // 持有收益（接口获取后计算）
    double holding_earning_rate = 0;  // 持有收益率（接口获取后计算）
    double holding_amount = 0;        // 持有金额（接口获取后计算）
    double nav = 0;                   // 单位净值（接口获取）
    double nav_gains = 0;             // 净值涨跌（接口获取）
    QString nav_time = "";            // 净值时间（接口获取）
    double valuation = 0;             // 盘中估值（接口获取）
    double valuation_gains = 0;       // 估值涨跌（接口获取）
    QString valuation_time = "";      // 估值时间（接口获取）
    double expected_earnings = 0;     // 预计收益（接口获取后计算）
    QString remarks = ""; // 备注（用户设定）
};

#endif // FUNDINFO_H
