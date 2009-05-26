#include "DomainInfo.h"
#include "ui_DomainInfo.h"

DomainInfo::DomainInfo(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::DomainInfoWidget)
{
    m_ui->setupUi(this);
}

void DomainInfo::addNewLine(const UrlInfo & data)
{
    int pos = m_ui->tableWidget_urls->rowCount();
    m_ui->tableWidget_urls->setRowCount(pos + 1);

    m_ui->tableWidget_urls->setItem(pos, 0, new QTableWidgetItem(data.url().toString()));
    m_ui->tableWidget_urls->setItem(pos, 1, new QTableWidgetItem(data.state() == UrlInfo::UrlOk ? data.stateString() : data.desc()));
    m_ui->tableWidget_urls->setItem(pos, 2, new QTableWidgetItem(QString::number(data.code())));
    m_ui->tableWidget_urls->setItem(pos, 3, new QTableWidgetItem(data.type()));
    m_ui->tableWidget_urls->setItem(pos, 4, new QTableWidgetItem(QString("%1KB").arg(data.length() / 1024)));
    m_ui->tableWidget_urls->setItem(pos, 5, new QTableWidgetItem(data.parent().toString()));
}

void DomainInfo::updateView(int total, int checked)
{
    m_ui->lineEdit_UrlTotal->setText(QString::number(total));
    m_ui->lineEdit_UrlChecked->setText(QString::number(checked));
    m_ui->lineEdit_UrlDifferent->setText(QString::number(m_cache.count(UrlInfo::UrlError)));
}

void DomainInfo::setData(const QList<UrlInfo> & data, QString state, int total)
{
    m_cache.clear();

    m_ui->tableWidget_urls->clearContents();
    m_ui->tableWidget_urls->setRowCount(0);

    m_ui->lineEdit_DNSStatus->setText(state);
    m_ui->tableWidget_urls->setSortingEnabled(false);
    for (int i = 0; i < data.size(); i++)
    {
        const UrlInfo & result = data.at(i);

        m_cache.insertMulti(result.state(), result);

        if (result.state() != UrlInfo::UrlNotChecked)
        {
            if (result.state() != UrlInfo::UrlOk)
            {
                addNewLine(result);
            }
            else
                if (m_ui->checkBox_ViewShowAll->isChecked())
                    addNewLine(result);
        }
    }
    m_ui->tableWidget_urls->setSortingEnabled(true);
    updateView(total, m_cache.size());
}

void DomainInfo::updateData(const UrlInfo & data, int total)
{
    m_cache.insertMulti(data.state(), data);
    m_ui->tableWidget_urls->setSortingEnabled(false);
    if (data.state() != UrlInfo::UrlOk)
    {
        addNewLine(data);
    }
    else
        if (m_ui->checkBox_ViewShowAll->isChecked())
            addNewLine(data);
    m_ui->tableWidget_urls->setSortingEnabled(true);
    updateView(total, m_cache.size());
}

void DomainInfo::clearData()
{
    m_ui->lineEdit_DNSStatus->clear();
    m_ui->lineEdit_UrlChecked->clear();
    m_ui->lineEdit_UrlDifferent->clear();
    m_ui->lineEdit_UrlTotal->clear();

    m_ui->tableWidget_urls->clearContents();
    m_ui->tableWidget_urls->setRowCount(0);
}

void DomainInfo::on_checkBox_ViewShowAll_toggled(bool checked)
{
    m_ui->tableWidget_urls->clearContents();
    m_ui->tableWidget_urls->setRowCount(0);

    m_ui->tableWidget_urls->setSortingEnabled(false);
    foreach(const UrlInfo & result, m_cache)
    {
        if (result.isChecked())
        {
            if (result.state() != UrlInfo::UrlOk)
            {
                addNewLine(result);
            }
            else
                if (checked)
                    addNewLine(result);
        }
    }
    m_ui->tableWidget_urls->setSortingEnabled(true);
}

void DomainInfo::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

DomainInfo::~DomainInfo()
{
    delete m_ui;
}

void DomainInfo::on_checkBox_ViewShowAll_stateChanged(int )
{

}
