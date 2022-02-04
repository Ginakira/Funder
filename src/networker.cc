#include "include/networker.h"

class NetWorker::Private {
public:
    explicit Private(NetWorker *q) : manager(new QNetworkAccessManager(q)) {}

    QNetworkAccessManager *manager;
};

NetWorker::NetWorker(QObject *parent)
        : QObject(parent), d(new NetWorker::Private(this)) {
    connect(d->manager, &QNetworkAccessManager::finished, this,
            &NetWorker::finished);
}

NetWorker::~NetWorker() {
    delete d;
    d = nullptr;
}

NetWorker *NetWorker::instance() {
    static NetWorker netWorker;
    return &netWorker;
}

QNetworkReply *NetWorker::get(const QString &url) {
    return d->manager->get(QNetworkRequest(QUrl(url)));
}

QNetworkReply *NetWorker::get_with_referer(const QString &url, QByteArray &&referer_url) {
    auto *request = new QNetworkRequest;
    request->setUrl(QUrl(url));
    request->setRawHeader("Referer", referer_url);
    return d->manager->get(*request);
}
