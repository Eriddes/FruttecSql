#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if(QTreeWidgetItem* headerFruttec = ui->treeWidget->headerItem())
    {
          headerFruttec->setText(0, tr("Codigo"));
          headerFruttec->setText(1, tr("Tabla"));
          headerFruttec->setText(2, tr("Registro"));
          headerFruttec->setText(3, tr("Orden"));
          headerFruttec->setText(4, tr("Estado"));
          headerFruttec->setText(5, "");
          ui->treeWidget->setAlternatingRowColors(true);
          ui->treeWidget->setSortingEnabled(true);
          ui->treeWidget->sortByColumn(4, Qt::DescendingOrder);
    }
    fruttecGet = new FruttecGet();
    threadGet = new QThread();
    fruttecGet->moveToThread(threadGet);
    connect(threadGet, SIGNAL(started()), fruttecGet, SLOT(runThread()));
    connect(fruttecGet, SIGNAL(processRows(QMap<QString,QVariant>)), this, SLOT(processRows(QMap<QString,QVariant>)));
    threadGet->start();

}
void MainWindow::processRows(QMap<QString,QVariant> mapRows)
{
    if(mapRows.size()>0)
    {
        //if((mapRows.contains("TABLE")) && (mapRows.contains("DATA")))
        QList<QTreeWidgetItem*> list = ui->treeWidget->findItems(mapRows["ID"].toString(), Qt::MatchExactly, 0);
        if(list.size()==0)
           addToTree(ui->treeWidget, mapRows);
    }
}
void MainWindow::addToTree(QTreeWidget *tree, QMap<QString,QVariant> mapRow)
{

    QTreeWidgetItem * item = new QTreeWidgetItem();
    item->setText(0,mapRow["ID"].toString());
    item->setText(1,mapRow["REF"].toString());
    item->setText(2,mapRow["COD"].toString());
    item->setText(3,mapRow["ORD"].toString());
    if(mapRow["OK_CFG"].toBool())
        item->setIcon(4,QIcon(":/img/img/information.png"));
    else
        item->setIcon(4,QIcon(":/img/img/clock_pause.png"));

    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    //tree->addTopLevelItem(item);
    tree->insertTopLevelItem ( 0, item );

    tree->resizeColumnToContents(0);
    tree->resizeColumnToContents(1);
    tree->resizeColumnToContents(2);
    tree->resizeColumnToContents(3);
    tree->resizeColumnToContents(4);

   //int nRows = tree->topLevelItemCount();
   /*int nMax=100;
   if(nRows>nMax)
   {
       for(int i=0; i<nRows; i++)
       {
           delete tree->topLevelItem(i);
       }
   }*/
}
MainWindow::~MainWindow()
{
    for(int i=0; i < QSqlDatabase::connectionNames().size(); i++)
    {
        QSqlDatabase sqlTmp = QSqlDatabase::database(QSqlDatabase::connectionNames().at(i));
        qDebug() << "        -DB ->" << sqlTmp.connectionName();
        sqlTmp.close();
    }
    delete ui;
}
