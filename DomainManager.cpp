#include "DomainManager.h"

DomainManager::DomainManager(const QString & domain, const QHostAddress & host)
{
    m_cache.insert(QUrl(domain));
}

void DomainManager::check(WebManager * manager)
{
    if (!m_cache.isEmpty())
    {
        QUrl url(*m_cache.constBegin());
    }
}
