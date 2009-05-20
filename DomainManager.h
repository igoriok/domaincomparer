#ifndef DOMAINMANAGER_H
#define DOMAINMANAGER_H

#include <QObject>
#include <QtCore/QSet>
#include <QHostAddress>
#include "UrlInfo.h"
#include "WebManager.h"

class DomainManager : public QObject
{
public:
    DomainManager(const QString & domain, const QHostAddress & host);

    void check(WebManager * manager);

signals:
    void ready();

private:
    QSet<QUrl> m_cache;
    QSet<UrlInfo> m_urls;
};

#endif // DOMAINMANAGER_H
