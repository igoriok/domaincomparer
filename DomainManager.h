#ifndef DOMAINMANAGER_H
#define DOMAINMANAGER_H

#include <QObject>
#include <QHostAddress>
#include "UrlInfo.h"
#include "WebManager.h"

class DomainManager : public QObject
{
    Q_OBJECT

public:
    DomainManager(const QString & domain, const QHostAddress & host);
    DomainManager(const DomainManager & other);
    DomainManager & operator =(const DomainManager & other);

    const QSet<UrlInfo> & result() const { return m_urls; }

public slots:
    void check(WebManager * manager);

signals:
    void ready();

private:
    QString m_domain;
    QHostAddress m_host;
    QHash<QUrl, QUrl> m_check;
    QSet<UrlInfo> m_urls;
    QHash<QUrl, QUrl> m_cache;

    QPair<QUrl, QUrl> current;
    WebManager * manager;

    void findNewUrls(const QString & html);

protected slots:
    void on_manager_ready(const WebResponse & live, const WebResponse & prev);
};

#endif // DOMAINMANAGER_H
