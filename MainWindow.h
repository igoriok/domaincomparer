#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QLabel>
#include "DatabaseManager.h"
#include "DomainManager.h"

namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindowClass *ui;

    DatabaseManager * m_db;
    DomainManager * dmanager;
    QModelIndex m_index;

    QLabel * current_domain;
    QLabel * current_checked;
    QLabel * current_total;

protected:
    void updateListModel(QAbstractItemModel * model);
    void updateTableModel(QAbstractItemModel * model);
    void updateDomainInfo(const DomainInfo & domainInfo);

protected slots:
    void on_dmanager_checking();
    void on_dmanager_ready();
    void on_dmanager_checked(const UrlInfo & ui);

private slots:
    void on_comboBox_currentIndexChanged(int index);
    void on_listView_Domains_clicked(QModelIndex index);
    void on_actionSkip_triggered();
    void on_actionStart_triggered();
    void on_actionStop_triggered();
    void on_actionAppend_triggered();
    void on_actionClear_triggered();
};

#endif // MAINWINDOW_H
