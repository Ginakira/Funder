//
// Created by Sakata on 2021/6/21.
//

#include "include/proxy_model.h"

#include <QColor>
#include <QDate>
#include <QDebug>

#include "include/database.h"

ProxyModel::ProxyModel(QObject *parent) : QIdentityProxyModel(parent) {}

QVariant ProxyModel::data(const QModelIndex &index, int role) const {
//    const QModelIndex source_index = mapToSource(index);
    int col = index.column();
    if (col == NAV_TIME_COL) { // 净值时间列
        if (role == Qt::DisplayRole) { // 净值时间不显示年份
            QString text = sourceModel()->data(index).toString();
            text = text.mid(5); // 截取掉年份
            return text;
        } else if (role == Qt::TextColorRole) { // 净值时间颜色
            QModelIndex nav_gains_index = sourceModel()->index(index.row(), NAV_GAINS_COL);
            double rate = sourceModel()->data(nav_gains_index).toDouble();
            if (rate < 0) {
                return QVariant::fromValue(QColor(Qt::darkGreen));
            } else {
                return QVariant::fromValue(QColor(Qt::red));
            }
        }
    } else if (col == NAV_GAINS_COL ||
               col == VALUATION_GAINS_COL) { // 净值涨跌、估值涨跌列
        if (role == Qt::DisplayRole) { // 正负号、百分号、保留两位
            double rate = sourceModel()->data(index).toDouble();
            return QString("%1%2%").arg(rate >= 0 ? "+" : "",
                                        QString::number(rate, 'f', 2));
        } else if (role == Qt::TextColorRole) { // 颜色
            double rate = sourceModel()->data(index).toDouble();
            if (rate < 0) {
                return QVariant::fromValue(QColor(Qt::darkGreen));
            } else {
                return QVariant::fromValue(QColor(Qt::red));
            }
        }
    } else if (col == HOLDING_EARNINGS_COL) { // 持仓收益列列
        if (role == Qt::DisplayRole) { // 保留两位
            double rate = sourceModel()->data(index).toDouble();
            return QString::number(rate, 'f', 2);
        } else if (role == Qt::TextColorRole) { //颜色
            double rate = sourceModel()->data(index).toDouble();
            if (rate < 0) {
                return
                        QVariant::fromValue(QColor(Qt::darkGreen)
                        );
            } else {
                return
                        QVariant::fromValue(QColor(Qt::red)
                        );
            }
        }
    } else if (col == NAV_COL) { // 净值列
        if (role == Qt::TextColorRole) { // 颜色
            QModelIndex nav_gains_index = sourceModel()->index(index.row(), NAV_GAINS_COL);
            double rate = sourceModel()->data(nav_gains_index).toDouble();
            if (rate < 0) {
                return
                        QVariant::fromValue(QColor(Qt::darkGreen)
                        );
            } else {
                return
                        QVariant::fromValue(QColor(Qt::red)
                        );
            }
        }
    } else if (col == VALUATION_COL) { // 估值列
        if (role == Qt::TextColorRole) { // 颜色
            QModelIndex valuation_gains_index = sourceModel()->index(index.row(), VALUATION_GAINS_COL);
            double rate = sourceModel()->data(valuation_gains_index).toDouble();
            if (rate < 0) {
                return
                        QVariant::fromValue(QColor(Qt::darkGreen)
                        );
            } else {
                return
                        QVariant::fromValue(QColor(Qt::red)
                        );
            }
        }
    } else if (col == HOLDING_TOTAL_COST_COL) { // 持有金额列
        if (role == Qt::DisplayRole) { // 保留整数
            return QString::number(sourceModel()->data(index).toDouble(), 'f', 0);
        }
    } else if (col == REMARKS_COL) { // 备注列
        if (role == Qt::ToolTipRole) { // 鼠标悬停提示
            return sourceModel()->data(index).toString();
        }
    } else if (col == EXPECTED_EARNINGS_COL) { // 预计收益列
        const QModelIndex nav_time_index = sourceModel()->index(index.row(), NAV_TIME_COL);
        const QModelIndex valuation_time_index = sourceModel()->index(index.row(), VALUATION_TIME_COL);
        QDate nav_date = QDate::fromString(sourceModel()->data(nav_time_index).toString(), "yyyy-MM-dd");
        QDate valuation_date = QDate::fromString(sourceModel()->data(valuation_time_index).toString().mid(0, 10),
                                                 "yyyy-MM-dd");
        double rate = 0;
        bool settled = false; // 是否已结算
        if (nav_date >= valuation_date) { // 已结算
            settled = true;
            const QModelIndex nav_index = sourceModel()->index(index.row(), NAV_COL);
            const QModelIndex nav_gains_index = sourceModel()->index(index.row(), NAV_GAINS_COL);
            const QModelIndex holding_share_index = sourceModel()->index(index.row(), HOLDING_SHARE_COL);
            double nav = sourceModel()->data(nav_index).toDouble();
            double nav_gains = sourceModel()->data(nav_gains_index).toDouble() / 100;
            double holding_share = sourceModel()->data(holding_share_index).toDouble();
            rate = holding_share * (nav / (1 + nav_gains) * nav_gains);
        } else {
            rate = sourceModel()->data(index).toDouble();
        }
        if (role == Qt::DisplayRole) { // 保留两位
            if (settled) {
                return QString("已结算：%1%2").arg(rate >= 0 ? "+" : "",
                                               QString::number(rate, 'f', 2));
            } else {
                return QString("%1%2").
                        arg(rate >= 0 ? "+" : "",
                            QString::number(rate, 'f', 2));
            }
        } else if (role == Qt::TextColorRole) { //颜色
            if (rate < 0) {
                return QVariant::fromValue(QColor(Qt::darkGreen));
            } else {
                return QVariant::fromValue(QColor(Qt::red));
            }
        }
    } else if (col == HOLDING_EARNING_RATE_COL) { // 持有收益率列
        if (role == Qt::DisplayRole) { // 百分号、保留两位
            double rate = sourceModel()->data(index).toDouble();
            return QString::number(rate, 'f', 2) + "%";
        } else if (role == Qt::TextColorRole) { // 颜色
            double rate = sourceModel()->data(index).toDouble();
            if (rate < 0) {
                return QVariant::fromValue(QColor(Qt::darkGreen));
            } else {
                return QVariant::fromValue(QColor(Qt::red));
            }
        }
    }
    return QIdentityProxyModel::data(index, role);
}