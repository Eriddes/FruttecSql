#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThreadPool>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QTreeWidget>
#include "fruttecget.h"


class FruttecGet;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

public slots:
    void processRows(QMap<QString,QVariant> lstRows);
private:
    Ui::MainWindow *ui;
    FruttecGet *fruttecGet;
    QThread *threadGet;

    void addToTree(QTreeWidget *tree, QMap<QString, QVariant> mapRow);
};

#endif // MAINWINDOW_H
