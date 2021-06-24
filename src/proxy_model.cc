//
// Created by Sakata on 2021/6/21.
//

#include "include/proxy_model.h"

#include <QColor>

#include "include/database.h"

ProxyModel::ProxyModel(QObject *parent) : QIdentityProxyModel(parent) {}

QVariant ProxyModel::data(const QModelIndex &index, int role) const {
//    const QModelIndex source_index = mapToSource(index);
    if ((index.column() == NAV_TIME_COL ||
         index.column() == VALUATION_TIME_COL) && role == Qt::DisplayRole) {
        // 净值时间、估值时间不显示年份
        QString text = sourceModel()->data(index).toString();
        text = text.mid(5); // 截取掉年份
        return text;
    } else if ((index.column() == HOLDING_EARNINGS_COL ||
                index.column() == HOLDING_EARNING_RATE_COL ||
                index.column() == NAV_GAINS_COL ||
                index.column() == VALUATION_GAINS_COL ||
                index.column() == EXPECTED_EARNINGS_COL) &&
               role == Qt::TextColorRole) {
        // 持仓收益、收益率、净值涨跌、估值涨跌、预计收益颜色
        double rate = sourceModel()->data(index).toDouble();
        if (rate < 0) {
            return QVariant::fromValue(QColor(Qt::darkGreen));
        } else {
            return QVariant::fromValue(QColor(Qt::red));
        }
    } else if ((index.column() == HOLDING_EARNING_RATE_COL ||
                index.column() == NAV_GAINS_COL ||
                index.column() == VALUATION_GAINS_COL) &&
               role == Qt::DisplayRole) {
        // 收益率、净值涨跌、估值涨跌百分号和保留两位
        double rate = sourceModel()->data(index).toDouble();
        return QString("%1%2%").arg(rate >= 0 ? "+" : "",
                                    QString::number(rate, 'f', 2));
    } else if ((index.column() == NAV_COL ||
                index.column() == NAV_TIME_COL)
               && role == Qt::TextColorRole) {
        // 单位净值、净值时间颜色
        QModelIndex nav_gains_index = sourceModel()->index(index.row(), NAV_GAINS_COL);
        double rate = sourceModel()->data(nav_gains_index).toDouble();
        if (rate < 0) {
            return QVariant::fromValue(QColor(Qt::darkGreen));
        } else {
            return QVariant::fromValue(QColor(Qt::red));
        }
    } else if (index.column() == VALUATION_COL && role == Qt::TextColorRole) {
        // 盘中估值颜色
        QModelIndex valuation_gains_index = sourceModel()->index(index.row(), VALUATION_GAINS_COL);
        double rate = sourceModel()->data(valuation_gains_index).toDouble();
        if (rate < 0) {
            return QVariant::fromValue(QColor(Qt::darkGreen));
        } else {
            return QVariant::fromValue(QColor(Qt::red));
        }
    } else if ((index.column() == HOLDING_EARNINGS_COL ||
                index.column() == EXPECTED_EARNINGS_COL)
               && role == Qt::DisplayRole) {
        // 持有收益、预计收益保留两位
        double rate = sourceModel()->data(index).toDouble();
        return QString("%1%2").arg(rate >= 0 ? "+" : "",
                                   QString::number(rate, 'f', 2));
    } else if ((index.column() == HOLDING_TOTAL_COST_COL) && role == Qt::DisplayRole) {
        // 持有金额保留整数
        return QString::number(sourceModel()->data(index).toDouble(), 'f', 0);
    } else {
        return QIdentityProxyModel::data(index, role);
    }
}