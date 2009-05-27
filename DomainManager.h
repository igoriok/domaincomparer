#ifndef DOMAINMANAGER_H
#define DOMAINMANAGER_H

#include <QHostAddress>
#include "UrlInfo.h"
#include "WebManager.h"

class DomainManager : public QObject
{
    Q_OBJECT

public:
    // Конструкторы
    DomainManager(QObject * parent = NULL);
    DomainManager(const DomainManager & other);
    DomainManager & operator =(const DomainManager & other);

    // Состояние DNS
    enum LiveState
    {
        LiveOk,
        LiveNotFound,
        LiveSwitched
    };

    void init(const QString & domain, const QHostAddress & host);

    // Геттеры
    const QString & domain() const { return m_domain; }
    QString host() const { return m_host.toString(); }
    const QSet<UrlInfo> & result() const { return m_urls; }
    const QUrl & current() const { return m_current.url(); }
    int total() const { return m_check.size() + m_urls.size(); }
    int count() const { return m_urls.size(); }
    int count(UrlInfo::UrlState state) const;
    LiveState state() const { return m_state; }
    QString stateString() const;

    // Setters
    void setLimit(int depth = 0, int count = 0, int max_count = 0);

public slots:
    // Внешние слоты
    void check();
    void abort();

signals:
    // Сигналы
    void checked(const UrlInfo & ui);
    void ready();

private:
    // Базовая информация
    QString m_domain;
    QHostAddress m_host;
    LiveState m_state;

    // Ограничения
    int m_depth_limit;
    int m_count_limit;
    int m_max_count_limit;

    // Данные ссылок
    QSet<UrlInfo> m_check;
    QSet<UrlInfo> m_urls;

    // Переменные проверки
    UrlInfo m_current;
    WebManager * manager;
    bool m_busy;

    // Вспомогательные функции
    void checkNext();
    void findNewUrls(const QUrl & parent, const QString & html);
    QString & replaceSpec(QString & str);

protected slots:
    // Внутренние слоты
    void on_manager_ready(const WebResponse & live, const WebResponse & prev);
};

#endif // DOMAINMANAGER_H
