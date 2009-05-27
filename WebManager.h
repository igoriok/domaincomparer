#ifndef WEBMANAGER_H
#define WEBMANAGER_H

#include <QtNetwork/QNetworkReply>
#include <QHostAddress>
#include <QSet>
#include "WebResponse.h"

class WebManager : public QObject
{
    Q_OBJECT

public:
    WebManager(QObject * parent = NULL);
    ~WebManager();

    void get(const QUrl & url, const QHostAddress & host = QHostAddress());
    void abort();

    static bool compareHost(QString orig, QString comp);

signals:
    void sending(const QString & method, const QUrl & url);
    void ready(const WebResponse & live, const WebResponse & prev);

private:
    QNetworkAccessManager * manager;
    QNetworkReply * live;
    QNetworkReply * prev;

    WebResponse _temp;
    QString _domain;
    QSet<QString> _ext;
    int _timer;

protected slots:
    void on_manager_finished(QNetworkReply * reply);
};

#endif // WEBMANAGER_H
