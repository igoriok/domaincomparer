#include "WebManager.h"

WebManager::WebManager()
{
    manager = new QNetworkAccessManager(this);

    _isBusy = false;
    _ext<<QString(".pdf")<<QString(".doc");
}

void WebManager::get(const QUrl & url)
{

}

void WebManager::on_manager_finished(QNetworkReply * reply)
{
}
