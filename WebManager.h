#ifndef WEBMANAGER_H
#define WEBMANAGER_H

#include <QObject>
#include <QSet>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include "WebResponse.h"

class WebManager : public QObject
{
public:
    WebManager();

    void get(const QUrl & url);

private:
    QNetworkAccessManager * manager;

    WebResponse * _temp;
    bool _isBusy;
    QList<QString> _ext;

protected slots:
    void on_manager_finished(QNetworkReply * reply);
};

#endif // WEBMANAGER_H
