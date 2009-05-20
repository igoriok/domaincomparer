#include "WebManager.h"
#include <QVariant>

WebManager::WebManager()
{
    manager = new QNetworkAccessManager(this);

    _isBusy = false;
    _ext<<QString(".pdf")<<QString(".doc");
}

void WebManager::get(const QUrl & url, const QHostAddress & host)
{
    QUrl purl(url);
    purl.setHost(host.toString());
    QNetworkRequest req1(url),
                    req2(purl);
    req1.setAttribute(QNetworkRequest::User, QVariant(true));
    req2.setRawHeader(QByteArray("Host"), url.host().toAscii());

    if (_ext.contains(url.path().right(4).toLower()))
    {
        // Sending HEAD request
        if (host.isNull())
        {
            req1.setAttribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 1), QVariant(1));
            manager->head(req1);
        }
        else
        {
            manager->head(req1);
            manager->head(req2);
        }
    }
    else
    {
        // Sendign GET request
        if (host.isNull())
        {
            req1.setAttribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 1), QVariant(1));
            manager->get(req1);
        }
        else
        {
            manager->get(req1);
            manager->get(req2);
        }
    }
}

void WebManager::on_manager_finished(QNetworkReply * reply)
{
    QNetworkRequest request(reply->request());
    bool isLive = request.attribute(QNetworkRequest::User).toBool();

    // Get Response Type
    QString type;
    if (reply->header(QNetworkRequest::ContentTypeHeader).type() == QVariant::String)
        type = reply->header(QNetworkRequest::ContentTypeHeader).toString();
    else
        type = reply->rawHeader(QByteArray("Content-Type"));

    // Get Response Data
    QByteArray data(reply->readAll());

    // Get Response Length
    qlonglong length = 0;
    if (reply->header(QNetworkRequest::ContentLengthHeader).type() == QVariant::LongLong)
        length = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    else
        length = data.length();

    WebResponse current(type, reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), data, length);

    if (_temp.isNull() && request.attribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 1)).isNull())
    {
        _temp = current;
    }
    else
    {
        if (isLive)
            emit ready(current, _temp);
        else
            emit ready(_temp, current);
        _temp.clear();
    }
}

WebManager::~WebManager()
{
}
