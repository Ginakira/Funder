//
// Created by Sakata on 2021/6/24.
//

#include "include/my_sql_table_model.h"

#include <QMimeData>
#include <QStandardItem>
#include <QSqlRecord>
#include <QDebug>
#include <QSqlError>

#include "include/database.h"

MySqlTableModel::MySqlTableModel(QObject *parent) : QSqlTableModel(parent) {
}

bool MySqlTableModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                               const QModelIndex &destinationParent, int destinationChild) {
    Q_UNUSED(sourceParent);
    Q_UNUSED(destinationParent);

    QModelIndex source_index = index(sourceRow, ORDER_ID_COL);
    QModelIndex dest_index = index(destinationChild, ORDER_ID_COL);
    int source_order = data(source_index).toInt();
    int dest_order = data(dest_index).toInt();

    if (source_order < dest_order) { // 上方向下拖动
        for (int i = 0; i < rowCount(); ++i) {
            QModelIndex order_index = index(i, ORDER_ID_COL);
            int original_order = data(order_index).toInt();
            if (original_order > source_order && original_order < dest_order) {
                setData(order_index, original_order - 1);
                submit();
            }
        }
        setData(source_index, dest_order - 1);
        submit();
    } else { // 下方向上拖动
        for (int i = 0; i < rowCount(); ++i) {
            QModelIndex order_index = index(i, ORDER_ID_COL);
            int original_order = data(order_index).toInt();
            if (original_order >= dest_order && original_order < source_order) {
                setData(order_index, original_order + 1);
                submit();
            }
        }
        setData(source_index, dest_order);
        submit();
    }
    select();
    return true;
}

Qt::ItemFlags MySqlTableModel::flags(const QModelIndex &index) const {
    if (index.isValid()) {
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QSqlTableModel::flags(index);
    }
    return QSqlTableModel::flags(index);
}

Qt::DropActions MySqlTableModel::supportedDropActions() const {
    return Qt::MoveAction | QSqlTableModel::supportedDropActions();
}

QMimeData *MySqlTableModel::mimeData(const QModelIndexList &indexes) const {
    QMimeData *data = QSqlTableModel::mimeData(indexes);
    if (data) {
        // parent mimeData中已判断indexes有效性，无效的会返回nullptr
        // 也可以把信息放到model的mutable成员中
        data->setData("row", QByteArray::number(indexes.at(0).row()));
        data->setData("col", QByteArray::number(indexes.at(0).column()));
    }
    return data;
}

bool MySqlTableModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                   const QModelIndex &parent) {
    if (!data || action != Qt::MoveAction) {
        return false;
    }

    // 这里没有判断toint ok（数据转换有效性）
    const QModelIndex old_index = index(data->data("row").toInt(),
                                        data->data("col").toInt());
    const QModelIndex current_index = parent;
    //可以先对index有效性进行判断，无效返回false，此处略过
    if (!old_index.isValid() || !current_index.isValid()) {
        return false;
    }
    moveRows(QModelIndex(), old_index.row(), 1, QModelIndex(), current_index.row());
    return true;
}
