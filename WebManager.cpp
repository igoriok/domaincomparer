#include "WebManager.h"
#include <QVariant>
#include <QStringList>

WebManager::WebManager(QObject * parent):
        QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    this->connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(on_manager_finished(QNetworkReply*)));

    manager->setCookieJar(NULL);

    live = NULL;
    prev = NULL;
    _timer = 0;

    _ext    <<QString(".pdf")<<QString(".doc")<<QString(".zip")<<QString(".tar")<<QString(".gz2")
            <<QString(".swf")<<QString(".exe")<<QString(".jpg")<<QString(".png")<<QString(".gif")
            <<QString(".wav")<<QString(".mp3")<<QString(".wma")<<QString(".wmv")<<QString(".mov")
            <<QString(".avi");
}

void WebManager::get(const QUrl & url, const QHostAddress & host)
{
    _domain = url.host();

    QUrl purl(url);
    purl.setHost(host.toString());
    QNetworkRequest req1(url),
                    req2(purl);
    req2.setRawHeader(QByteArray("Host"), url.host().toAscii());
    QString method;

    if (_ext.contains(url.path().right(4).toLower()))
    {
        // Sending HEAD request
        method.append("HEAD");
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
        method.append("GET");
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
    //_timer = this->startTimer(3000);
    emit sending(method, url);
}

void WebManager::abort()
{
    if (live != NULL)
    {
        live->abort();
        delete live;
        live = NULL;
    }
    if (prev != NULL)
    {
        prev->abort();
        delete prev;
        prev = NULL;
    }

    _temp.clear();
    _domain.clear();
}

bool WebManager::compareHost(QString orig, QString comp)
{
    orig.toLower();
    comp.toLower();
    if (orig.startsWith(QString("www.")))
        orig.remove(0, 4);
    if (comp.startsWith(QString("www.")))
        comp.remove(0, 4);
    return (orig == comp);
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
        delete reply;
    }
    else
    {
        // Get Response Type
        QString type;
        if (reply->header(QNetworkRequest::ContentTypeHeader).type() == QVariant::String)
            type = reply->header(QNetworkRequest::ContentTypeHeader).toString();
        else
            type = reply->rawHeader(QByteArray("Content-Type"));

        // Get Response Location
        QUrl redirect;
        if (reply->header(QNetworkRequest::LocationHeader).type() == QVariant::Url)
            redirect = reply->header(QNetworkRequest::LocationHeader).toUrl();
        else
            if (reply->hasRawHeader("Location"))
                redirect = request.url().resolved(QUrl(QString(reply->rawHeader("Location"))));

        // Get Response Data
        QByteArray data(reply->readAll());

        // Get Response Length
        qlonglong length = 0;
        if (reply->header(QNetworkRequest::ContentLengthHeader).type() == QVariant::LongLong)
            length = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
        else
            length = data.length();

        WebResponse current(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), type.split(QChar(';')).at(0), redirect, data, length);
        delete reply;

        if (((current.code() / 100) == 3) && compareHost(_domain, current.location().host()))
        {
            QUrl url(request.url());
            url.setPath(current.location().path());
            request.setUrl(url);
            if (_ext.contains(url.path().right(4).toLower()))
            {
                if (isLive)
                    live = manager->head(request);
                else
                    prev = manager->head(request);
            }
            else
            {
                if (isLive)
                    live = manager->get(request);
                else
                    prev = manager->get(request);
            }
        }
        else
        {
            if (_temp.isNull() && request.attribute(QNetworkRequest::User).isNull())
            {
                _temp = current;
            }
            else
            {
                _domain.clear();
                if (isLive)
                    emit ready(current, _temp);
                else
                    emit ready(_temp, current);
                _temp.clear();
            }
        }
    }
}

WebManager::~WebManager()
{
    abort();
}
