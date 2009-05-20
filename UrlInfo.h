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
    bool operator ==(const UrlInfo & other);
    static UrlInfo compare(const WebResponse & prev, const WebResponse & live);
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
