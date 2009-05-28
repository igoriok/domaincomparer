#include "DomainInfo.h"

DomainInfo::DomainInfo():
        m_null(true)
{
}

DomainInfo::DomainInfo(const QString & domain, const QString & host, DomainState state):
        m_domain(domain), m_host(host), m_state(state), m_null(false)
{
}

void DomainInfo::setInfo(const QString & domain, const QString & host, DomainState state)
{
    m_domain = domain;
    if (!m_domain.startsWith(QString("www.")))
        m_domain.insert(0, QString("www."));

    m_host = host;
    m_state = state;
    m_null = false;
}

void DomainInfo::setState(DomainState state)
{
    if (!m_null)
        m_state = state;
}

void DomainInfo::clear()
{
    m_domain.clear();
    m_host.clear();
    m_state = DomainNotChecked;
    m_null = true;
}

QString DomainInfo::stateString(DomainInfo::DomainState state)
{
    QString str;
    switch(state)
    {
        case DomainNotChecked:
            str.append("NOT CHECKED");
            break;
        case DomainOk:
            str.append("OK");
            break;
        case DomainNotFound:
            str.append("NOT FOUND");
            break;
        case DomainSwitched:
            str.append("SWITCHED");
            break;
    }
    return str;
}
