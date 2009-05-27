#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QMap>
#include "DomainManager.h"

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    DatabaseManager(QObject * parent = NULL);
    ~DatabaseManager();

    bool open();

    void insertDomain(const QString & domain, DomainManager::LiveState state);
    void insertDomainResult(const QString & domain, const QSet<UrlInfo> & result);
    QMap<UrlInfo::UrlState, int> getDomainStatistic(const QString & domain);
    QSqlQueryModel * getDomainModel(const QString & domain);

private:
    QSqlDatabase m_db;
    QHash<QString, int> m_cache;
};

#endif // DATABASEMANAGER_H
