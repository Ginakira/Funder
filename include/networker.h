#ifndef NETWORKER_H
#define NETWORKER_H

#include <QObject>
#include <QtNetwork>

class NetWorker : public QObject {
Q_OBJECT
public:
    static NetWorker *instance();

    ~NetWorker() override;

    NetWorker(const NetWorker &) Q_DECL_EQ_DELETE;

    NetWorker &operator=(NetWorker rhs) Q_DECL_EQ_DELETE;

    QNetworkReply *get(const QString &url);

    QNetworkReply *get_with_referer(const QString &url, const QString &referer_url,
                                    const std::function<void(QNetworkRequest *)> &request_modifier = nullptr);

signals:

    void finished(QNetworkReply *reply);

private:
    class Private;

    friend class Private;

    Private *d;

    explicit NetWorker(QObject *parent = nullptr);

};

#endif // NETWORKER_H
