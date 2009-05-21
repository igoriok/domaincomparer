#include "DomainInfo.h"
#include "ui_DomainInfo.h"

DomainInfo::DomainInfo(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::DomainInfoWidget)
{
    m_ui->setupUi(this);
    m_different = 0;
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
    m_ui->lineEdit_UrlDifferent->setText(QString::number(m_different));
}

void DomainInfo::setData(const QList<UrlInfo> & data, QString state, int total)
{
    setData(data, state, total, false);
}

void DomainInfo::setData(const QList<UrlInfo> & data, QString state, int total, bool update)
{
    if (!update)
    {
        m_cache.clear();
        m_cache.append(data);
    }

    m_ui->tableWidget_urls->clearContents();
    m_ui->tableWidget_urls->setRowCount(0);

    m_different = 0;
    m_ui->lineEdit_DNSStatus->setText(state);
    m_ui->tableWidget_urls->setSortingEnabled(false);
    for (int i = 0; i < data.size(); i++)
    {
        const UrlInfo & result = data.at(i);
        if (result.isChecked())
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
    m_ui->tableWidget_urls->setSortingEnabled(false);
    updateView(total, m_cache.size());
}

void DomainInfo::updateData(const UrlInfo & data, int total)
{
    m_cache.append(data);
    if (data.state() != UrlInfo::UrlOk)
    {
        m_different++;
        addNewLine(data);
    }
    else
        if (m_ui->checkBox_ViewShowAll->isChecked())
            addNewLine(data);
    updateView(total, m_cache.size());
}

void DomainInfo::clearData()
{
}

void DomainInfo::on_checkBox_ViewShowAll_toggled(bool checked)
{
    m_ui->tableWidget_urls->clearContents();
    m_ui->tableWidget_urls->setRowCount(0);

    m_ui->tableWidget_urls->setSortingEnabled(false);
    for (int i = 0; i < m_cache.size(); i++)
    {
        const UrlInfo & result = m_cache.at(i);
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
    m_ui->tableWidget_urls->setSortingEnabled(false);
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
