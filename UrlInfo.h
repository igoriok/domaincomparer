#ifndef URLINFO_H
#define URLINFO_H

#include <QtCore/QUrl>
#include <QHash>
#include "WebResponse.h"

class UrlInfo
{
public:
    UrlInfo();

    UrlInfo(const UrlInfo & other);
    UrlInfo & operator =(const UrlInfo & other);
    bool operator ==(const UrlInfo & other) const;
    static UrlInfo compare(const QUrl & url, const QUrl & parent, const WebResponse & live, const WebResponse & prev);
    friend uint qHash(const UrlInfo & ui);

    bool isValid() const { return m_valid; }

private:
    QUrl m_url;
    QUrl m_parent;
    int m_code;
    QString m_type;

    bool m_valid;
};

extern uint qHash(const UrlInfo & ui);
extern uint qHash(const QUrl & url);

#endif // URLINFO_H
