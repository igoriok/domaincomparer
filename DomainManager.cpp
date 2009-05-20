#include "DomainManager.h"

DomainManager::DomainManager(const QString & domain, const QHostAddress & host)
{
    m_domain = domain.toLower();
    if (m_domain.startsWith(QString("www.")))
        m_domain.remove(0, 4);

    m_host = host;
    m_check.insert(QUrl(domain), QUrl());
}

DomainManager::DomainManager(const DomainManager & other)
{
    m_domain = other.m_domain;
    m_host = other.m_host;
    m_check.insert(QUrl(m_domain), QUrl());
}

DomainManager & DomainManager::operator =(const DomainManager & other)
{
    m_check.clear();
    m_urls.clear();
    m_cache.clear();

    m_domain = other.m_domain;
    m_host = other.m_host;
    m_check.insert(QUrl(m_domain), QUrl());
    return *this;
}

void DomainManager::check(WebManager * manager)
{
    if (!m_check.isEmpty())
    {
        QHash<QUrl, QUrl>::iterator iter = m_check.begin();
        current = QPair<QUrl, QUrl>(iter.key(), iter.value());
        m_check.erase(iter);

        this->connect(manager, SIGNAL(ready(WebResponse,WebResponse)), SLOT(on_manager_ready(WebResponse, WebResponse)));
        manager->get(current.first, m_host);
    }
    else
    {
        m_cache.clear();
        current = QPair<QUrl, QUrl>();

        manager->disconnect(this);
        manager = NULL;

        emit ready();
    }
}

void DomainManager::findNewUrls(const QString & html)
{
}

void DomainManager::on_manager_ready(const WebResponse & live, const WebResponse & prev)
{
    m_urls.insert(UrlInfo::compare(current.first, current.second, live, prev));
    check(manager);
}
