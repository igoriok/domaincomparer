#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
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

    DomainManager * dmanager;
    WebManager * wmanager;

private:
    Ui::MainWindowClass *ui;

protected slots:
    void on_dmanager_ready();
};

#endif // MAINWINDOW_H
