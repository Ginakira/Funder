//
// Created by Sakata on 2021/6/21.
//

#ifndef FUNDER_PROXY_MODEL_H
#define FUNDER_PROXY_MODEL_H

#include <QIdentityProxyModel>

class ProxyModel : public QIdentityProxyModel {
public:
    explicit ProxyModel(QObject *parent = nullptr);

    ~ProxyModel() override = default;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};


#endif //FUNDER_PROXY_MODEL_H
