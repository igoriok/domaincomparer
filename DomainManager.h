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
    DomainManager(const QString & domain, const QHostAddress & host, QObject * parent = NULL);
    DomainManager(const DomainManager & other);
    DomainManager & operator =(const DomainManager & other);

    // Состояние DNS
    enum LiveState
    {
        LiveOk,
        LiveNotFound,
        LiveSwitched
    };

    // Геттеры
    const QSet<UrlInfo> & result() const { return m_urls; }
    const QUrl & current() const { return m_current.url(); }
    int total() const { return m_check.size() + m_urls.size(); }
    int count() const { return m_urls.size(); }
    int count(UrlInfo::UrlState state) const;
    LiveState state() const { return m_state; }
    QString stateString() const;

public slots:
    // Внешние слоты
    void check(WebManager * manager);
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

    // Вспомогательные функции
    void checkNext();
    void findNewUrls(const QUrl & parent, const QString & html);
    QString & replaceSpec(QString & str);
    bool compareHost(QString orig, QString comp);

protected slots:
    // Внутренние слоты
    void on_manager_ready(const WebResponse & live, const WebResponse & prev);
};

#endif // DOMAINMANAGER_H
