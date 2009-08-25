#include "DomainHelper.h"
#include <QRegExp>

DomainHelper::DomainHelper()
{
}

bool DomainHelper::compareHost(QString first, QString second)
{
    first.toLower();
    second.toLower();
    if (first.startsWith(QString("www.")))
        first.remove(0, 4);
    if (second.startsWith(QString("www.")))
        second.remove(0, 4);
    return (first == second);
}

QList<UrlInfo> DomainHelper::findNewUrls(const QUrl & parent, const QString & html)
{
    QList<UrlInfo> urlList;

    int pos = 0;
    QRegExp reg("(?:href|src|background|action)[\\s]*=(?:[\\s]*(?:\"[^\"]+\"|'[^']+')|[^'\"\\s>]*)", Qt::CaseInsensitive);

    while ((pos = reg.indexIn(html, pos)) != -1)
    {
        QString str(reg.cap(0));
        //qDebug(QString("FINDED %1").arg(str).toAscii());

        if (str.endsWith('"')) {
            int pos = str.indexOf('"') + 1;
            str = str.mid(pos, str.size() - pos - 1);
        } else if (str.endsWith('\'')) {
            int pos = str.indexOf('\'') + 1;
            str = str.mid(pos, str.size() - pos - 1);
        } else if (str.indexOf('=') != -1) {
            int pos = str.indexOf('=') + 1;
            str = str.mid(pos, str.size() - pos);
        } else str.clear();
        if (!str.isEmpty())
        {
            replaceSpec(str);
            str = QUrl::fromPercentEncoding(str.toAscii());
            str = str.trimmed();
            //str.remove(QRegExp("^[\\d]*;(URL=)?", Qt::CaseInsensitive));

            if (!str.contains(QChar('\'')) &&
                !str.contains(QChar('"')) &&
                !str.contains(QChar('[')) &&
                str != QString("\\") &&
                str.size() > 0)
            {
                QUrl url;
                if (str.startsWith(QString("http://")))
                    url = QUrl(str);
                else if (str.startsWith(QChar('#')))
                    url = QUrl();
                else
                    url = parent.resolved(QUrl(str));

                if (url.hasFragment()) url.setFragment(QString());
                if (url.path().isEmpty()) url.setPath(QString("/"));

                if (url.isValid() && compareHost(parent.host(), url.host())) {
                    url.setHost(parent.host());
                    UrlInfo ui(url, parent);
                    urlList.append(ui);
                } else {
                    //qDebug(QString("SKIP %1").arg(url.toString()).toAscii());
                }
            }
        }

        pos += reg.matchedLength();
    }

    return urlList;
}

QString & DomainHelper::replaceSpec(QString & str)
{
    return str.replace(QString("&amp;"), QString("&"), Qt::CaseInsensitive)
            .replace(QString("&gt;"), QString(">"), Qt::CaseInsensitive)
            .replace(QString("&lt;"), QString("<"), Qt::CaseInsensitive);
}
