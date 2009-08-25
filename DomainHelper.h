#ifndef DOMAINHELPER_H
#define DOMAINHELPER_H

#include <QString>
#include <QUrl>
#include "UrlInfo.h"

class DomainHelper
{
private:
    DomainHelper();
    
public:
    static bool compareHost(QString firstm, QString second);
    static QList<UrlInfo> findNewUrls(const QUrl & parent, const QString & html);
    static QString & replaceSpec(QString & str);
};

#endif // DOMAINHELPER_H
