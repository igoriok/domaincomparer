#include <QSqlQuery>
#include <QSqlError>
#include "DatabaseManager.h"

DatabaseManager::DatabaseManager(QObject * parent):
        QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", "db_sqlite");
    m_db.setDatabaseName("db.s3db");
}

bool DatabaseManager::open()
{
    bool isOpen = false;
    isOpen = m_db.open();
    if (isOpen)
    {
        QSqlQuery q(m_db);
        q.exec("SELECT name, id FROM domains");
        while(q.next())
            m_cache.insert(q.value(0).toString(), q.value(1).toInt());
        q.finish();
        //q.exec("DELETE FROM domains");
        //q.exec("DELETE FROM urls");
    }
    return isOpen;
}

void DatabaseManager::close()
{
    if (m_db.isOpen())
        m_db.close();
}

void DatabaseManager::insertDomain(const DomainInfo & domainInfo)
{
    QString domain(domainInfo.domain());
    QSqlQuery q(m_db);
    if (!m_cache.contains(domain))
    {
        q.prepare("INSERT INTO domains (name, host, state) VALUES (:name, :host, :state)");
        q.bindValue(":name", domain);
        q.bindValue(":host", domainInfo.host());
        q.bindValue(":state", (int)domainInfo.state());
        if (!q.exec())
            qDebug("Error inserting domain");

        q.exec(QString("SELECT DISTINCT id FROM domains WHERE name = '%1'").arg(domain));
        if (q.first())
            m_cache.insert(domain, q.value(0).toInt());
        else
            qDebug("Domain not found!");
    }
    else
    {
        q.prepare("UPDATE domains SET state = :state WHERE name = :name");
        q.bindValue(":state", (int)domainInfo.state());
        q.bindValue(":name", domain);
        q.exec();
        q.finish();
    }
    q.finish();
}

void DatabaseManager::insertDomainResult(const QString & domain, const QSet<UrlInfo> & result)
{
    if (m_cache.contains(domain))
    {
        int id = m_cache.value(domain);
        QSqlQuery q(m_db);
        q.exec(QString("DELETE FROM links WHERE domain = %1").arg(id));
        foreach(const UrlInfo & info, result)
        {
            q.prepare("INSERT INTO links (url, domain, state, code, type, length, parent, desc) VALUES (:url, :domain, :state, :code, :type, :length, :parent, :desc)");
            q.bindValue(":url", info.url().toString());
            q.bindValue(":domain", id);
            q.bindValue(":state", (int)info.state());
            q.bindValue(":code", info.code());
            q.bindValue(":type", info.type());
            q.bindValue(":length", info.length());
            q.bindValue(":parent", info.parent().toString());
            q.bindValue(":desc", info.desc());
            q.exec();
        }
        q.finish();
    }
}

void DatabaseManager::clearDomains()
{
    clearResults();
    m_db.exec("DELETE FROM domains");
    m_cache.clear();
}

void DatabaseManager::clearResults()
{
    m_db.exec("DELETE FROM links");
}

DomainInfo DatabaseManager::getDomainInfo(const QString & domain)
{
    DomainInfo domainInfo;
    if (m_cache.contains(domain))
    {
        QSqlQuery q(m_db);
        q.prepare("SELECT host, state FROM domains WHERE name = :name");
        q.bindValue(":name", domain);
        q.exec();
        if (q.first())
            domainInfo = DomainInfo(domain, q.value(0).toString(), (DomainInfo::DomainState)q.value(1).toInt());

        q.finish();
    }

    return domainInfo;
}

QMap<UrlInfo::UrlState, int> DatabaseManager::getDomainStatistic(const QString & domain)
{
    QMap<UrlInfo::UrlState, int> map;
    if (m_cache.contains(domain))
    {
        QSqlQuery q(QString("SELECT state, COUNT(*) FROM links WHERE domain = %1 GROUP BY (state)").arg(m_cache.value(domain)), m_db);
        while (q.next())
            map.insert((UrlInfo::UrlState)q.value(0).toInt(), q.value(1).toInt());
        q.finish();
    }
    return map;
}

QAbstractItemModel * DatabaseManager::getDomainsModel()
{
    QSqlQueryModel * model = new QSqlQueryModel();
    model->setQuery("SELECT name FROM domains ORDER BY name ASC", m_db);
    return model;

}

QAbstractItemModel * DatabaseManager::getLinksModel(const QString & domain, UrlInfo::UrlState state)
{
    QSqlTableModel * model = new QSqlTableModel(NULL, m_db);
    model->setTable("links_view");
    switch (state)
    {
        case UrlInfo::UrlOk:
            model->setFilter(QString("DOMAIN = '%1' AND STATE = 'OK'").arg(domain));
            break;
        case UrlInfo::UrlWarning:
            model->setFilter(QString("DOMAIN = '%1' AND STATE = 'WARNING'").arg(domain));
            break;
        case UrlInfo::UrlError:
            model->setFilter(QString("DOMAIN = '%1' AND STATE = 'ERROR'").arg(domain));
            break;
        case UrlInfo::UrlNotChecked:
            model->setFilter(QString("DOMAIN = '%1'").arg(domain));
            break;
    }
    model->setSort(0, Qt::AscendingOrder);
    model->select();
    model->removeColumn(0);
    return model;
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen())
        m_db.close();
}
