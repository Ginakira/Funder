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


QNetworkReply *NetWorker::get_with_referer(const QString &url, const QString &referer_url,
                                           const std::function<void(QNetworkRequest *)> &request_modifier) {
    auto *request = new QNetworkRequest;
    request->setUrl(QUrl(url));
    request->setRawHeader("Referer", referer_url.toUtf8());
    request->setRawHeader("X-Requested-With", "XMLHttpRequest");
    request->setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader,
                       "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/16.0 Safari/605.1.15");

    if (request_modifier != nullptr) {
        request_modifier(request);
    }
    return d->manager->get(*request);
}
