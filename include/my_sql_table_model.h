//
// Created by Sakata on 2021/6/24.
//

#ifndef FUNDER_MY_SQL_TABLE_MODEL_H
#define FUNDER_MY_SQL_TABLE_MODEL_H

#include <QSqlTableModel>


class MySqlTableModel : public QSqlTableModel {
Q_OBJECT
public:
    explicit MySqlTableModel(QObject *parent = nullptr);

    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                  const QModelIndex &destinationParent, int destinationChild) override;

    // 允许的操作，加上drag drop
    Qt::ItemFlags flags(const QModelIndex &index) const override;


    // 允许move
    Qt::DropActions supportedDropActions() const override;

    // drag时携带的信息
    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    // drop时根据drag携带的信息进行处理
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                      const QModelIndex &parent) override;
};


#endif //FUNDER_MY_SQL_TABLE_MODEL_H
