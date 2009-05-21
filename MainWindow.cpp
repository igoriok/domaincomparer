#include <QMessageBox>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_AddRange.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    wmanager = new WebManager(this);
    wmanager->setObjectName("wmanager");

    ui->setupUi(this);

    m_current = NULL;
}

void MainWindow::on_wmanager_sending(const QString & method, const QUrl & url)
{
    ui->statusBar->showMessage(QString("%1 %2").arg(method).arg(url.toString()));
}

void MainWindow::dmanager_ready()
{
    if (m_current != NULL)
    {
        DomainManager * dmanager = m_childs.value(m_current);
        dmanager->disconnect(this);
    }

    int index = (m_current == NULL) ? -1 : ui->domains->row(m_current);
    index++;

    if (index < ui->domains->count())
    {
        m_current = ui->domains->item(index);

        DomainManager * dmanager = m_childs.value(m_current);
        this->connect(dmanager, SIGNAL(ready()), SLOT(dmanager_ready()));
        this->connect(dmanager, SIGNAL(checked(UrlInfo)), SLOT(dmanager_checked(UrlInfo)));

        dmanager->check(wmanager);
    }
    else
    {
        m_current = NULL;
        ui->actionCheck->setEnabled(true);
    }
}

void MainWindow::dmanager_checked(const UrlInfo & ui)
{
    if (m_current == this->ui->domains->currentItem())
    {
        this->ui->domainInfo->updateData(ui, m_childs.value(m_current)->total(), m_childs.value(m_current)->count());
    }
}

void MainWindow::on_actionClear_triggered()
{
    foreach(DomainManager * manager, m_childs)
    {
        delete manager;
    }
    m_childs.clear();
    ui->domains->clear();
}

void MainWindow::on_domains_itemSelectionChanged()
{
    QListWidgetItem * item = ui->domains->currentItem();
    if (item != NULL)
    {
        DomainManager * manager = m_childs.value(item);
        ui->domainInfo->setData(manager->result().toList(), manager->stateString(), manager->total(), manager->count());
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
                m_childs.insert(item, dmanageer);
            }
        }
        else
        {
            QMessageBox::warning(this, QString("Error"), QString("You forgote set host!"));
        }
    }
}

void MainWindow::on_actionStart_triggered()
{
    if (m_childs.size() > 0)
    {
        ui->actionStart->setEnabled(false);
        m_current = NULL;

        dmanager_ready();
    }
}

void MainWindow::on_actionStop_triggered()
{
    if (m_current != NULL)
    {
        m_childs.value(m_current)->abort();
        m_childs.value(m_current)->disconnect(this);

        m_current = NULL;
        ui->actionStart->setEnabled(true);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
