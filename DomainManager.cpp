#include <QHostInfo>
#include <QRegExp>
#include "DomainManager.h"

DomainManager::DomainManager(QObject * parent):
        QObject(parent), m_depth_limit(0), m_count_limit(0), m_max_count_limit(0)
{
    this->manager = new WebManager(this);
    this->connect(manager, SIGNAL(ready(WebResponse,WebResponse)), SLOT(on_manager_ready(WebResponse, WebResponse)));
    m_busy = false;
}

DomainManager::DomainManager(const DomainManager & other):
        QObject(NULL)
{
    m_depth_limit = other.m_depth_limit;
    m_count_limit = other.m_count_limit;
    m_max_count_limit = other.m_max_count_limit;

    m_domainInfo = other.m_domainInfo;
}

DomainManager & DomainManager::operator =(const DomainManager & other)
{
    m_check.clear();
    m_urls.clear();

    init(other.m_domainInfo);
    return *this;
}

void DomainManager::init(const DomainInfo & domain)
{
    if (m_busy)
    {
        abort();
        m_busy = false;
    }

    m_domainInfo = domain;

    QList<QHostAddress> add = QHostInfo::fromName(m_domainInfo.domain()).addresses();
    if (add.size() == 0)
        m_domainInfo.setState(DomainInfo::DomainNotFound);
    else if (add.at(0).toString() == m_domainInfo.host())
        m_domainInfo.setState(DomainInfo::DomainSwitched);
    else
        m_domainInfo.setState(DomainInfo::DomainOk);

    m_check.clear();
    m_urls.clear();
}

int DomainManager::count(UrlInfo::UrlState state) const
{
    int count = 0;
    for(QSet<UrlInfo>::const_iterator iter = m_urls.constBegin(); iter != m_urls.constEnd(); iter++)
    {
        if ((*iter).state() == state)
            count++;
    }
    return count;
}

void DomainManager::setLimit(int depth, int count, int max_count)
{
    if (depth >= 0) m_depth_limit = depth;
    if (count >= 0) m_count_limit = count;
    if (max_count >= 0) m_max_count_limit = max_count;
}

void DomainManager::check()
{
    if (!m_busy)
    {
        m_busy = true;
        m_urls.clear();
        m_check.clear();
        m_check.insert(UrlInfo(QUrl(QString("http://%1/").arg(m_domainInfo.domain())), QUrl()));

        checkNext();
    }
}

void DomainManager::checkNext()
{
    if (!m_check.isEmpty())
    {
        QSet<UrlInfo>::iterator iter = m_check.begin();
        m_current = (*iter);
        m_check.erase(iter);

        emit checking(m_current.url(), m_check.size() + m_urls.size(), m_urls.size());
        manager->get(m_current.url(), QHostAddress(m_domainInfo.host()));
    }
    else
    {
        m_current.clear();
        m_busy = false;

        emit ready();
    }
}

void DomainManager::abort()
{
    if (m_busy)
    {
        manager->abort();
        m_busy = false;
    }

    m_current.clear();
}

void DomainManager::findNewUrls(const QUrl & parent, const QString & html)
{
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

                if (url.isValid() && WebManager::compareHost(m_domainInfo.domain(), url.host())) {
                    url.setHost(m_domainInfo.domain());
                    UrlInfo ui(url, parent);
                    if (!m_check.contains(ui) && !m_urls.contains(ui))
                    {
                        m_check.insert(ui);
                        if (m_max_count_limit == (m_check.size() + m_urls.size() + 1))
                            break;
                    }
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

void DomainManager::on_manager_ready(const WebResponse & live, const WebResponse & prev)
{
    m_current.compare(live, prev);
    m_urls.insert(m_current);

    if (((live.code() / 100) == 2) &&
        live.type().startsWith(QString("text/")))
    {
        int depth = 0;
        if (m_depth_limit > 0)
        {
            UrlInfo ui(m_current.parent(), QUrl());
            while (!ui.url().isEmpty())
            {
                QSet<UrlInfo>::const_iterator f = m_urls.constFind(ui);
                if (f == m_urls.constEnd())
                    ui = UrlInfo((*m_check.constFind(ui)).parent(), QUrl());
                else
                    ui = UrlInfo((*f).parent(), QUrl());
                depth++;
            }
        }

        int cnt = m_urls.size() + m_check.size();

        if ((m_max_count_limit == 0 || cnt < m_max_count_limit) && // если не перевалило максимум
            ((m_count_limit == 0) || (cnt < m_count_limit) || (cnt >= m_count_limit && depth <= m_depth_limit))) // или перевалило но глубина позволяет
        {
            QByteArray data(live.data());
            data.replace('\0', ' ');
            findNewUrls(m_current.url(), QString(data));
        }
    }

    if ((live.code() / 100) == 3)
    {
        QUrl redirect(live.location());
        if (WebManager::compareHost(m_domainInfo.domain(), redirect.host()) || WebManager::compareHost(m_domainInfo.host(), redirect.host()))
        {
            redirect.setHost(m_domainInfo.domain());
            UrlInfo ui(redirect, m_current.parent());
            if (!m_check.contains(ui) && !m_urls.contains(ui))
                m_check.insert(ui);
        }
    }

    emit checked(m_current);
    checkNext();
}
