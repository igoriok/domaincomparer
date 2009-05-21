#ifndef DOMAININFO_H
#define DOMAININFO_H

#include <QtGui/QWidget>
#include "DomainManager.h"

namespace Ui {
    class DomainInfoWidget;
}

class DomainInfo : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(DomainInfo)
public:
    explicit DomainInfo(QWidget *parent = 0);
    virtual ~DomainInfo();

    void setData(const QList<UrlInfo> & data, QString state, int total);
    void updateData(const UrlInfo & data, int total);
    void clearData();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::DomainInfoWidget *m_ui;
    int m_different;
    QList<UrlInfo> m_cache;

    void addNewLine(const UrlInfo & data);
    void updateView(int total, int checked);

private slots:
    void on_checkBox_ViewShowAll_toggled(bool checked);
    void setData(const QList<UrlInfo> & data, QString state, int total, bool update);
};

#endif // DOMAININFO_H
