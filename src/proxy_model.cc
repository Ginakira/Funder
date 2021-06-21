//
// Created by Sakata on 2021/6/21.
//

#include "include/proxy_model.h"

#include <QColor>

ProxyModel::ProxyModel(QObject *parent) : QIdentityProxyModel(parent) {}

QVariant ProxyModel::data(const QModelIndex &index, int role) const {
    const QModelIndex source_index = mapToSource(index);
    if ((source_index.column() == 11 ||
         source_index.column() == 14) && role == Qt::DisplayRole) {
        // 净值时间、估值时间不显示年份
        QString text = sourceModel()->data(source_index).toString();
        text = text.mid(5);
        return text;
    } else if ((source_index.column() == 6 ||
                source_index.column() == 7 ||
                source_index.column() == 10 ||
                source_index.column() == 13 ||
                source_index.column() == 15) &&
               role == Qt::TextColorRole) {
        // 持仓收益、收益率、净值涨跌、估值涨跌、预计收益颜色
        double rate = sourceModel()->data(source_index).toDouble();
        if (rate < 0) {
            return QVariant::fromValue(QColor(Qt::darkGreen));
        } else {
            return QVariant::fromValue(QColor(Qt::red));
        }
    } else if ((source_index.column() == 7 ||
                source_index.column() == 10 ||
                source_index.column() == 13) &&
               role == Qt::DisplayRole) {
        // 收益率、净值涨跌、估值涨跌百分号和保留两位
        double rate = sourceModel()->data(source_index).toDouble();
        return QString::number(rate, 'f', 2) + "%";
    } else if (source_index.column() == 9 && role == Qt::TextColorRole) {
        // 单位净值颜色
        QModelIndex nav_gains_index = sourceModel()->index(source_index.row(), 10);
        double rate = sourceModel()->data(nav_gains_index).toDouble();
        if (rate < 0) {
            return QVariant::fromValue(QColor(Qt::darkGreen));
        } else {
            return QVariant::fromValue(QColor(Qt::red));
        }
    } else if (source_index.column() == 12 && role == Qt::TextColorRole) {
        // 盘中估值颜色
        QModelIndex valuation_gains_index = sourceModel()->index(source_index.row(), 13);
        double rate = sourceModel()->data(valuation_gains_index).toDouble();
        if (rate < 0) {
            return QVariant::fromValue(QColor(Qt::darkGreen));
        } else {
            return QVariant::fromValue(QColor(Qt::red));
        }
    } else if ((source_index.column() == 6 ||
                source_index.column() == 15)
               && role == Qt::DisplayRole) {
        // 持有收益、预计收益保留两位
        double rate = sourceModel()->data(source_index).toDouble();
        return QString::number(rate, 'f', 2);
    } else {
        return QIdentityProxyModel::data(index, role);
    }
}