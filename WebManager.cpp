#include "WebManager.h"
#include <QVariant>

WebManager::WebManager(QObject * parent):
        QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    this->connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(on_manager_finished(QNetworkReply*)));

    live = NULL;
    prev = NULL;

    _ext    <<QString(".pdf")<<QString(".doc")<<QString(".zip")<<QString(".tar")<<QString(".gz2")
            <<QString(".swf")<<QString(".exe")<<QString(".jpg")<<QString(".png")<<QString(".gif")
            <<QString(".wav")<<QString(".mp3")<<QString(".wma")<<QString(".wmv")<<QString(".mov")
            <<QString(".avi");
}

void WebManager::get(const QUrl & url, const QHostAddress & host)
{
    QUrl purl(url);
    purl.setHost(host.toString());
    QNetworkRequest req1(url),
                    req2(purl);
    req2.setRawHeader(QByteArray("Host"), url.host().toAscii());

    if (_ext.contains(url.path().right(4).toLower()))
    {
        // Sending HEAD request
        if (host.isNull())
        {
            req1.setAttribute(QNetworkRequest::User, QVariant(1));
            live = manager->head(req1);
        }
        else
        {
            live = manager->head(req1);
            prev = manager->head(req2);
        }
    }
    else
    {
        // Sendign GET request
        if (host.isNull())
        {
            req1.setAttribute(QNetworkRequest::User, QVariant(1));
            live = manager->get(req1);
        }
        else
        {
            live = manager->get(req1);
            prev = manager->get(req2);
        }
    }
}

void WebManager::abort()
{
    if (live != NULL)
    {
        live->abort();
        live = NULL;
    }
    if (prev != NULL)
    {
        prev->abort();
        prev = NULL;
    }
}

void WebManager::on_manager_finished(QNetworkReply * reply)
{
    QNetworkRequest request(reply->request());
    bool isLive = (reply == live) ? true : false;
    if (reply == live) live = NULL;
    else if (reply == prev) prev = NULL;

    if (reply->error() == QNetworkReply::OperationCanceledError)
    {
        _temp.clear();
    }
    else
    {
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

        if (_temp.isNull() && request.attribute(QNetworkRequest::User).isNull())
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
}

WebManager::~WebManager()
{
}
