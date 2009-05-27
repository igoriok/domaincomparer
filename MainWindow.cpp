#include <QMessageBox>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_AddRange.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    m_dbmanager = new DatabaseManager(this);
    m_dbmanager->open();

    m_dmanager = new DomainManager(this);

    ui->setupUi(this);

    m_current = NULL;
}

void MainWindow::on_wmanager_sending(const QString & method, const QUrl & url)
{
    ui->statusBar->showMessage(QString("%1 %2").arg(method).arg(url.toString()));
}

void MainWindow::dmanager_ready()
{
    m_dbmanager->insertDomainResult(dmanager->domain(), dmanager->result());

    if(dmanager->count(UrlInfo::UrlError) > 0)
        m_current->setBackgroundColor(QColor(Qt::red));
    else
        m_current->setBackgroundColor(QColor(Qt::green));

    int index = ui->domains->row(m_current) + 1;

    if (index < ui->domains->count())
    {
        m_current = ui->domains->item(index);

        DomainManager * dmanager = m_childs.value(m_current);
        this->connect(dmanager, SIGNAL(ready()), SLOT(dmanager_ready()));
        this->connect(dmanager, SIGNAL(checked(UrlInfo)), SLOT(dmanager_checked(UrlInfo)));
        m_current->setBackgroundColor(QColor(Qt::yellow));

        dmanager->check(wmanager);
    }
    else
    {
        m_current = NULL;
        ui->actionStart->setEnabled(true);
    }
}

void MainWindow::dmanager_checked(const UrlInfo & ui)
{
    if (m_current == this->ui->domains->currentItem())
    {
        //this->ui->domainInfo->updateData(ui, m_childs.value(m_current)->total());
    }
}

void MainWindow::on_domains_itemSelectionChanged()
{
    QListWidgetItem * item = ui->domains->currentItem();
    if (item != NULL)
    {
        if (m_childs.contains(item))
        {
            DomainManager * dmanager = m_childs.value(item);
            //ui->domainInfo->setData(dmanager->result().toList(), dmanager->stateString(), dmanager->total());

            QAbstractItemModel * mode = ui->tableView->model();
            ui->tableView->setModel(m_db->getDomainModel(dmanager->domain()));
            if (mode != NULL)
                delete mode;
        }
        else
            ui->domainInfo->clearData();
    }
    else
        ui->domainInfo->clearData();
}

void MainWindow::on_actionStart_triggered()
{
    if (m_childs.size() > 0)
    {
        ui->actionStart->setEnabled(false);
        m_current = ui->domains->item(0);
        m_current->setBackgroundColor(QColor(Qt::yellow));

        DomainManager * dmanager = m_childs.value(m_current);
        this->connect(dmanager, SIGNAL(ready()), SLOT(dmanager_ready()));
        this->connect(dmanager, SIGNAL(checked(UrlInfo)), SLOT(dmanager_checked(UrlInfo)));

        dmanager->check(wmanager);
        ui->actionStop->setEnabled(true);
    }
}


void MainWindow::on_actionSkip_triggered()
{
    if (m_current != NULL)
    {
        DomainManager * dmanager = m_childs.value(m_current);
        dmanager->abort();

        dmanager_ready();
    }
}

void MainWindow::on_actionStop_triggered()
{
    if (m_current != NULL)
    {
        DomainManager * dmanager = m_childs.value(m_current);
        dmanager->abort();
        dmanager->disconnect(this);

        m_current = NULL;
        ui->actionStart->setEnabled(true);
    }
}

void MainWindow::on_actionAppend_triggered()
{
    QDialog dlg(this);
    Ui::AddRange dui;
    dui.setupUi(&dlg);
    if (dlg.exec() == QDialog::Accepted)
    {
        QString host(dui.lineEdit->text().trimmed());
        if (host.size() > 0)
        {
            QStringList list(dui.plainTextEdit->toPlainText().split(QChar('\n'), QString::SkipEmptyParts));
            foreach(const QString & domain, list)
            {
                QListWidgetItem * item = new QListWidgetItem(domain);
                ui->domains->addItem(item);
                DomainManager * dmanageer = new DomainManager(domain, QHostAddress(host), this);
                dmanageer->setLimit(3, 300, 1000);
                m_childs.insert(item, dmanageer);

                m_db->insertDomain(dmanageer->domain(), dmanageer->state());
            }
        }
        else
        {
            QMessageBox::warning(this, QString("Error"), QString("You forgote set host!"));
        }
    }
}

void MainWindow::on_actionClear_triggered()
{
    if (m_current != NULL)
        on_actionStop_triggered();

    while(m_childs.size() > 0)
    {
        DomainManager * dmanager = m_childs.take(m_childs.constBegin().key());
        delete dmanager;
    }

    ui->domains->clear();
    ui->domainInfo->clearData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

