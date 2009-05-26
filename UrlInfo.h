#ifndef URLINFO_H
#define URLINFO_H

#include <QtCore/QUrl>
#include <QHash>
#include "WebResponse.h"

class UrlInfo
{
public:
    UrlInfo();
    UrlInfo(const QUrl & url, const QUrl & parent);

    UrlInfo(const UrlInfo & other);
    UrlInfo & operator =(const UrlInfo & other);
    bool operator ==(const UrlInfo & other) const;
    void compare(const WebResponse & live, const WebResponse & prev);

    enum UrlState
    {
        UrlNotChecked,
        UrlError,
        UrlWarning,
        UrlOk
    };

    const QUrl & url() const { return m_url; }
    const QUrl & parent() const { return m_parent; }
    int code() const { return m_code; }
    const QString & type() const { return m_type; }
    qlonglong length() const { return m_length; }
    UrlState state() const { return m_state; }
    const QString & desc() const { return m_desc; }

    bool isValid() const { return m_valid; }
    bool isChecked() const { return (m_state != UrlNotChecked); }
    QString stateString() const;
    void clear();

private:
    QUrl m_url;
    QUrl m_parent;
    int m_code;
    QString m_type;
    qlonglong m_length;
    UrlState m_state;
    QString m_desc;

    bool m_valid;
    //bool m_checked;

    friend uint qHash(const UrlInfo & ui);
};

extern uint qHash(const UrlInfo & ui);
extern uint qHash(const QUrl & url);

#endif // URLINFO_H
