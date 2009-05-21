#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    dmanager = new DomainManager("sandyswebdesign.com", QHostAddress(QString("216.251.43.17")), this);
    dmanager->setObjectName("dmanager");

    wmanager = new WebManager(this);
    wmanager->setObjectName("wmanager");

    ui->setupUi(this);

    dmanager->check(wmanager);
}

void MainWindow::on_dmanager_ready()
{
    ui->listWidget->clear();
    QSet<UrlInfo> urls(dmanager->result());
    foreach(const UrlInfo & url, urls)
    {
        ui->listWidget->addItem(url.url().toString());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
