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
    DomainManager(const QString & domain, const QHostAddress & host, QObject * parent = NULL);
    DomainManager(const DomainManager & other);
    DomainManager & operator =(const DomainManager & other);

    const QSet<UrlInfo> & result() const { return m_urls; }

    enum LiveState
    {
        LiveOk,
        LiveNotFound,
        LiveSwitched
    };

public slots:
    void check(WebManager * manager);

signals:
    void ready();

private:
    QString m_domain;
    QHostAddress m_host;
    LiveState m_state;

    QSet<UrlInfo> m_check;
    QSet<UrlInfo> m_urls;

    UrlInfo current;
    WebManager * manager;

    void checkNext();
    void findNewUrls(const QUrl & parent, const QString & html);
    QString & replaceSpec(QString & str);
    bool compareHost(QString orig, QString comp);

protected slots:
    void on_manager_ready(const WebResponse & live, const WebResponse & prev);
};

#endif // DOMAINMANAGER_H
