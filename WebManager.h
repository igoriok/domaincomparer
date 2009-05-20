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
    WebManager();
    ~WebManager();

    void get(const QUrl & url, const QHostAddress & host = QHostAddress());

signals:
    void ready(const WebResponse & live, const WebResponse & prev);

private:
    QNetworkAccessManager * manager;

    WebResponse _temp;
    bool _isBusy;
    QSet<QString> _ext;

protected slots:
    void on_manager_finished(QNetworkReply * reply);
};

#endif // WEBMANAGER_H
