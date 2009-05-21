#include <QHostInfo>
#include <QRegExp>
#include "DomainManager.h"

DomainManager::DomainManager(const QString & domain, const QHostAddress & host, QObject * parent):
        QObject(parent)
{
    m_domain = domain.toLower();
    if (!m_domain.startsWith(QString("www.")))
        m_domain.insert(0, QString("www."));

    m_host = host;
    QList<QHostAddress> add = QHostInfo::fromName(m_domain).addresses();
    if (add.size() == 0)
        m_state = LiveNotFound;
    else if (add.at(0) == m_host)
        m_state = LiveSwitched;
    else
        m_state = LiveOk;

    m_check.insert(UrlInfo(QUrl(QString("http://%1/").arg(m_domain)), QUrl()));
}

DomainManager::DomainManager(const DomainManager & other):
        QObject(NULL)
{
    DomainManager(other.m_domain, other.m_host);
}

DomainManager & DomainManager::operator =(const DomainManager & other)
{
    m_check.clear();
    m_urls.clear();

    DomainManager(other.m_domain, other.m_host);
    return *this;
}

void DomainManager::check(WebManager * manager)
{
    if (manager != NULL)
    {
        this->manager = manager;
        this->connect(manager, SIGNAL(ready(WebResponse,WebResponse)), SLOT(on_manager_ready(WebResponse, WebResponse)));

        checkNext();
    }
    else
    {
        emit ready();
    }
}

void DomainManager::findNewUrls(const QUrl & parent, const QString & html)
{
    int pos = 0;
    QRegExp reg("(?:meta|href|src|background|action)[\\s]*=(?:[\\s]*(?:\"[^\"]+\"|'[^']+')|[^'\"\\s>]*)", Qt::CaseInsensitive);

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
            str.remove(QRegExp("^[\\d]*;"));

            if (!str.contains(QChar('\''))
                && !str.contains(QChar('"'))
                && !str.contains(QChar('[')))
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

                if (url.isValid() && compareHost(m_domain, url.host())) {
                    url.setHost(m_domain);
                    UrlInfo ui(url, parent);
                    if (!m_check.contains(ui) && !m_urls.contains(ui))
                        m_check.insert(ui);
                } else {
                    //qDebug(QString("SKIP %1").arg(url.toString()).toAscii());
                }
            }
        }

        pos += reg.matchedLength();
    }
}

QString & DomainManager::replaceSpec(QString & str)
{
    return str.replace(QString("&amp;"), QString("&"), Qt::CaseInsensitive)
            .replace(QString("&gt;"), QString(">"), Qt::CaseInsensitive)
            .replace(QString("&lt;"), QString("<"), Qt::CaseInsensitive);
}

bool DomainManager::compareHost(QString orig, QString comp)
{
    orig.toLower();
    comp.toLower();
    if (orig.startsWith(QString("www.")))
        orig.remove(0, 4);
    if (comp.startsWith(QString("www.")))
        comp.remove(0, 4);
    return (orig == comp);
}

void DomainManager::checkNext()
{
    if (!m_check.isEmpty())
    {
        QSet<UrlInfo>::iterator iter = m_check.begin();
        current = (*iter);
        m_check.erase(iter);

        manager->get(current.url(), m_host);
    }
    else
    {
        current.clear();

        manager->disconnect(this);
        manager = NULL;

        emit ready();
    }
}

void DomainManager::on_manager_ready(const WebResponse & live, const WebResponse & prev)
{
    current.compare(live, prev);
    m_urls.insert(current);

    if (((live.code() / 100) == 2) &&
        live.type().startsWith(QString("text/")))
    {
        QByteArray data(live.data());
        data.replace('\0', ' ');
        findNewUrls(current.url(), QString(data));
    }
    qDebug(QString("Checked %1 - %2 - %3").arg(current.url().toString()).arg(current.stateString()).arg(current.code()).toAscii());

    checkNext();
}
