#include <QMessageBox>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_AddRange.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    m_db = new DatabaseManager(this);
    m_db->open();

    dmanager = new DomainManager(this);
    dmanager->setObjectName("dmanager");

    ui->setupUi(this);

    QAbstractItemModel * oldModel = ui->listView->model();
    ui->listView->setModel(m_db->getDomainsModel());
    delete oldModel;
}

void MainWindow::updateListModel(QAbstractItemModel * model)
{
    QAbstractItemModel * oldModel = ui->listView->model();
    ui->listView->setModel(model);
    delete oldModel;
}

void MainWindow::updateTableModel(QAbstractItemModel * model)
{
    QAbstractItemModel * oldModel = ui->tableView->model();
    ui->tableView->setModel(model);
    delete oldModel;
}

void MainWindow::on_dmanager_ready()
{
    m_db->insertDomain(dmanager->domainInfo());
    m_db->insertDomainResult(dmanager->domainInfo().domain(), dmanager->result());

    m_index = ui->listView->model()->index(m_index.row() + 1, 0);

    if (m_index.isValid())
    {
        dmanager->init(m_db->getDomainInfo(ui->listView->model()->data(m_index).toString()));
        dmanager->check();
    }
}

void MainWindow::on_dmanager_checked(const UrlInfo & ui)
{
    this->ui->statusBar->showMessage(ui.url().toString());
}

void MainWindow::on_actionStart_triggered()
{
    m_index = ui->listView->model()->index(0, 0);
    if (m_index.isValid())
    {
        dmanager->init(m_db->getDomainInfo(ui->listView->model()->data(m_index).toString()));
        dmanager->check();
    }
}

void MainWindow::on_actionSkip_triggered()
{
}

void MainWindow::on_actionStop_triggered()
{
    dmanager->abort();
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
                m_db->insertDomain(DomainInfo(domain, dui.lineEdit->text().trimmed()));
            }
            updateListModel(m_db->getDomainsModel());
        }
        else
        {
            QMessageBox::warning(this, QString("Error"), QString("You forgote set host!"));
        }
    }
}

void MainWindow::on_actionClear_triggered()
{
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_listView_clicked(QModelIndex index)
{
    if (index.isValid())
    {
        QAbstractItemModel * model = ui->listView->model();
        QString domain(model->data(index).toString());
        updateTableModel(m_db->getLinksModel(domain));
        ui->tableView->sortByColumn(1, Qt::DescendingOrder);
    }
}
