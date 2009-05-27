#include <QSqlQuery>
#include "DatabaseManager.h"

DatabaseManager::DatabaseManager(QObject * parent):
        QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", "ComparerDB");
    m_db.setDatabaseName("/home/igor/domain.db");
}

bool DatabaseManager::open()
{
    bool isOpen = false;
    isOpen = m_db.open();
    if (isOpen)
    {
        QSqlQuery q(m_db);
        q.exec("CREATE TABLE IF NOT EXISTS domains (id INTEGER PRIMARY KEY, name TEXT NOT NULL UNIQUE, state INTEGER NOT NULL)");
        q.exec("CREATE TABLE IF NOT EXISTS urls (id INTEGER PRIMARY KEY, domain INTEGER NOT NULL REFERENCES domains (id), url TEXT NOT NULL, state INTEGER NOT NULL, code TEXT NOT NULL)");

        q.exec("DELETE FROM domains");
        q.exec("DELETE FROM urls");
    }
    return isOpen;
}

void DatabaseManager::insertDomain(const QString & domain, DomainManager::LiveState state)
{
    QSqlQuery q(m_db);
    if (!m_cache.contains(domain))
    {
        q.prepare("INSERT INTO domains (name, state) VALUES (:name, :state)");
        q.bindValue(":name", domain);
        q.bindValue(":state", (int)state);
        q.exec();

        q.exec(QString("SELECT DISTINCT id FROM domains WHERE name = '%1'").arg(domain));
        if (q.first())
            m_cache.insert(domain, q.value(0).toInt());
    }
    else
    {
        q.prepare("UPDATE domain SET state = :state");
        q.bindValue(":state", (int)state);
        q.exec();
    }
    q.finish();
}

void DatabaseManager::insertDomainResult(const QString & domain, const QSet<UrlInfo> & result)
{
    if (m_cache.contains(domain))
    {
        int id = m_cache.value(domain);
        QSqlQuery q(QString("DELETE FROM urls WHERE domain = %1").arg(id), m_db);
        foreach(const UrlInfo & info, result)
        {
            q.prepare("INSERT INTO urls (domain, url, state, code) VALUES (:domain, :url, :state, :code)");
            q.bindValue(":domain", id);
            q.bindValue(":url", info.url().toString());
            q.bindValue(":state", (int)info.state());
            q.bindValue(":code", info.code());
            q.exec();
        }
        q.finish();
    }
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
    }
    return map;
}

QAbstractItemModel * DatabaseManager::getDomainModel(const QString & domain)
{
    QSqlQueryModel * model = new QSqlQueryModel();
    QSqlQuery q1(QString("SELECT DISTINCT id FROM domains WHERE name = '%1'").arg(domain), m_db);
    if (q1.first())
        model->setQuery(QString("SELECT url as URL, state as STATE FROM urls WHERE domain = %1").arg(q1.value(0).toInt()), m_db);
    return model;
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isValid())
    {
        if (m_db.isOpen())
            m_db.close();
        QSqlDatabase::removeDatabase("ComparerDB");
    }
}
