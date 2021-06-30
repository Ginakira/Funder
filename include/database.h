#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlTableModel>

#define ID_COL 0                   // ID
#define ORDER_ID_COL 1             // 显示顺序ID
#define CODE_COL 2                 // 基金代码
#define NAME_COL 3                 // 基金名称
#define HOLDING_UNIT_COST_COL 4    // 持仓成本
#define HOLDING_SHARE_COL 5        // 持有份额
#define HOLDING_TOTAL_COST_COL 6   // 成本金额
#define HOLDING_EARNINGS_COL 7     // 持有收益
#define HOLDING_EARNING_RATE_COL 8 // 持有收益率
#define HOLDING_AMOUNT_COL 9       // 持有总金额
#define NAV_COL 10                  // 单位净值
#define NAV_GAINS_COL 11           // 净值涨跌
#define NAV_TIME_COL 12            // 净值时间
#define VALUATION_COL 13           // 盘中估值
#define VALUATION_GAINS_COL 14     // 估值涨跌
#define VALUATION_TIME_COL 15      // 估值时间
#define EXPECTED_EARNINGS_COL 16   // 预计收益
#define REMARKS_COL 17             // 备注
#define SETTLED_COL 18             // 是否已结算

#define ID_KEY "id"                                      // ID
#define ORDER_ID_KEY "order_id"                         // 显示顺序ID
#define CODE_KEY "code"                                  // 基金代码
#define NAME_KEY "name"                                  // 基金名称
#define HOLDING_UNIT_COST_KEY "holding_unit_cost"        // 持仓成本
#define HOLDING_SHARE_KEY "holding_share"                // 持有份额
#define HOLDING_TOTAL_COST_KEY "holding_total_cost"      // 成本金额
#define HOLDING_EARNINGS_KEY "holding_earnings"          // 持有收益
#define HOLDING_EARNING_RATE_KEY "holding_earning_rate"  // 持有收益率
#define HOLDING_AMOUNT_KEY "holding_amount"              // 持有总金额
#define NAV_KEY "nav"                                    // 单位净值
#define NAV_GAINS_KEY "nav_gains"                        // 净值涨跌
#define NAV_TIME_KEY "nav_time"                          // 净值时间
#define VALUATION_KEY "valuation"                        // 盘中估值
#define VALUATION_GAINS_KEY "valuation_gains"            // 估值涨跌
#define VALUATION_TIME_KEY "valuation_time"              // 估值时间
#define EXPECTED_EARNINGS_KEY "expected_earnings"        // 预计收益
#define SETTLED_KEY "settled"                            // 是否已结算
#define REMARKS_KEY "remarks"                            // 备注

bool init_db_connect();

void set_db_model_header_data(QSqlTableModel *model);

int get_holding_table_rows();

bool create_table();

bool alter_table();

#endif // DATABASE_H
