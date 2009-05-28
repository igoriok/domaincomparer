#ifndef DOMAININFO_H
#define DOMAININFO_H

#include <QString>

class DomainInfo
{
public:
    enum DomainState
    {
        DomainNotChecked,
        DomainNotFound,
        DomainSwitched,
        DomainOk
    };

    DomainInfo();
    DomainInfo(const QString & domain, const QString & host, DomainState state = DomainNotChecked);

    const QString & domain() const { return m_domain; }
    const QString & host() const { return m_host; }
    DomainState state() const { return m_state; }
    bool isNull() const { return m_null; }

    void setInfo(const QString & domain, const QString & host, DomainState state = DomainNotChecked);
    void setState(DomainState state);

    void clear();

    static QString stateString(DomainState state);

private:
    QString m_domain;
    QString m_host;
    DomainState m_state;

    bool m_null;
};

#endif // DOMAININFO_H
