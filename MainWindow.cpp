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
    dmanager->setLimit(3, 300, 600);

    ui->setupUi(this);

    ui->comboBox->addItem("ALL", (int)UrlInfo::UrlNotChecked);
    ui->comboBox->addItem("ERROR", (int)UrlInfo::UrlError);
    ui->comboBox->addItem("WARNING", (int)UrlInfo::UrlWarning);
    ui->comboBox->addItem("OK", (int)UrlInfo::UrlOk);
    ui->comboBox->setCurrentIndex(0);

    current_domain = new QLabel(this);
    current_domain->setMinimumWidth(100);
    ui->statusBar->addPermanentWidget(current_domain);
    current_checked = new QLabel(this);
    current_checked->setMinimumWidth(20);
    ui->statusBar->addPermanentWidget(current_checked);
    current_total = new QLabel(this);
    current_total->setMinimumWidth(20);
    ui->statusBar->addPermanentWidget(current_total);

    QAbstractItemModel * oldModel = ui->listView_Domains->model();
    ui->listView_Domains->setModel(m_db->getDomainsModel());
    delete oldModel;
}

void MainWindow::updateListModel(QAbstractItemModel * model)
{
    QAbstractItemModel * oldModel = ui->listView_Domains->model();
    ui->listView_Domains->setModel(model);
    delete oldModel;
}

void MainWindow::updateTableModel(QAbstractItemModel * model)
{
    QAbstractItemModel * oldModel = ui->tableView_Links->model();
    ui->tableView_Links->setModel(model);
    delete oldModel;
}

void MainWindow::updateDomainInfo(const DomainInfo & domainInfo)
{
    ui->lineEdit_Host->setText(domainInfo.host());
    ui->lineEdit_State->setText(domainInfo.stateString());
}

void MainWindow::on_dmanager_checking(const QUrl & url, int total, int checked)
{
    ui->statusBar->showMessage(url.toString());
    current_total->setText(QString::number(total));
    current_checked->setText(QString::number(checked));
}

void MainWindow::on_dmanager_ready()
{
    m_db->insertDomain(dmanager->domainInfo());
    m_db->insertDomainResult(dmanager->domainInfo().domain(), dmanager->result());

    m_index = ui->listView_Domains->model()->index(m_index.row() + 1, 0);

    if (m_index.isValid())
    {
        dmanager->init(m_db->getDomainInfo(ui->listView_Domains->model()->data(m_index).toString()));
        dmanager->check();
    }
    else
        updateListModel(m_db->getDomainsModel());
}

void MainWindow::on_dmanager_checked(const UrlInfo & ui)
{
    this->ui->statusBar->showMessage(ui.url().toString());
}

void MainWindow::on_actionStart_triggered()
{
    m_index = ui->listView_Domains->model()->index(0, 0);
    if (m_index.isValid())
    {
        dmanager->init(m_db->getDomainInfo(ui->listView_Domains->model()->data(m_index).toString()));
        dmanager->check();
    }
}

void MainWindow::on_actionSkip_triggered()
{
    dmanager->abort();
    on_dmanager_ready();
}

void MainWindow::on_actionStop_triggered()
{
    dmanager->abort();
    m_index = QModelIndex();
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
    if (m_index.isValid())
        on_actionStop_triggered();

    m_db->clearDomains();
    updateListModel(m_db->getDomainsModel());
}

MainWindow::~MainWindow()
{
    QAbstractItemModel * model1 = ui->listView_Domains->model();
    QAbstractItemModel * model2 = ui->tableView_Links->model();
    delete ui;
    delete model1;
    delete model2;
    m_db->close();
}

void MainWindow::on_listView_Domains_clicked(QModelIndex index)
{
    if (index.isValid())
    {
        QAbstractItemModel * model = ui->listView_Domains->model();
        QString domain(model->data(index).toString());
        updateTableModel(m_db->getLinksModel(domain, (UrlInfo::UrlState)ui->comboBox->itemData(ui->comboBox->currentIndex()).toInt()));
        ui->tableView_Links->setSortingEnabled(true);

        QMap<UrlInfo::UrlState, int> map(m_db->getDomainStatistic(domain));
        ui->lineEdit->setText(QString::number(map.value(UrlInfo::UrlError)));
        ui->lineEdit_2->setText(QString::number(map.value(UrlInfo::UrlWarning)));
        ui->lineEdit_3->setText(QString::number(map.value(UrlInfo::UrlOk) + map.value(UrlInfo::UrlWarning) + map.value(UrlInfo::UrlError)));

        updateDomainInfo(m_db->getDomainInfo(domain));
    }
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    if (ui->listView_Domains->currentIndex().isValid())
        updateTableModel(m_db->getLinksModel(ui->listView_Domains->model()->data(ui->listView_Domains->currentIndex()).toString(), (UrlInfo::UrlState)ui->comboBox->itemData(index).toInt()));
}
