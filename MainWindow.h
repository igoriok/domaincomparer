#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QListWidgetItem>
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

    DatabaseManager * m_dbmanager;
    DomainManager * m_dmanager;
    QListWidgetItem * m_current;
    bool isBatch;

protected slots:
    void on_wmanager_sending(const QString & method, const QUrl & url);
    void dmanager_ready();
    void dmanager_checked(const UrlInfo & ui);

private slots:
    void on_actionSkip_triggered();
    void on_actionStart_triggered();
    void on_actionStop_triggered();
    void on_actionAppend_triggered();
    void on_actionClear_triggered();
    void on_domains_itemSelectionChanged();
};

#endif // MAINWINDOW_H
