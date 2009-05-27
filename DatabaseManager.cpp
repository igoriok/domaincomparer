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
        q.prepare("UPDATE domain SET state = :state");
        q.bindValue(":state", (int)domainInfo.state());
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
    QSqlQuery q1(QString("SELECT DISTINCT id FROM domains WHERE name = '%1'").arg(domain), m_db);
    if (q1.first())
    {
        QSqlQuery q(QString("SELECT state, COUNT(*) FROM urls WHERE domain = %1 GROUP BY (state)").arg(q1.value(0).toInt()), m_db);
        while (q.next())
            map.insert((UrlInfo::UrlState)q.value(0).toInt(), q.value(1).toInt());
        q.finish();
    }
    q1.finish();
    return map;
}

QAbstractItemModel * DatabaseManager::getDomainsModel()
{
    QSqlTableModel * model = new QSqlTableModel(NULL, m_db);
    model->setTable("domains_view");
    return model;
}

QAbstractItemModel * DatabaseManager::getLinksModel(const QString & domain)
{
    /*
    QSqlQueryModel * model = new QSqlQueryModel();
    QSqlQuery q(m_db);
    q.prepare("SELECT DISTINCT id FROM domains WHERE name = :name");
    q.bindValue(":name", domain);
    q.exec();
    if (q.first())
        model->setQuery(QString("SELECT l.url as URL, s.text as STATE FROM links as l LEFT JOIN lstates as s ON l.state = s.id WHERE l.domain = %1").arg(q.value(0).toInt()), m_db);
    else
        qDebug("Links model error");
    q.finish();
    return model;
    */
    QSqlTableModel * model = new QSqlTableModel(NULL, m_db);
    model->setTable("links_view");
    model->setFilter(QString("DOMAIN = '%1'").arg(domain));
    model->removeColumn(0);
    return model;
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen())
        m_db.close();
}
