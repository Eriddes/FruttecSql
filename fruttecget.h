#ifndef FRUTTECGET_H
#define FRUTTECGET_H

#include <QObject>
#include <QSqlDatabase>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QSqlError>
#include <QThreadPool>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDateTime>
#include <QTimer>
#include <QTime>
#include <QCoreApplication>
#include <QWaitCondition>
#include <QMutex>

#include "fruttecset.h"

//prueba 2

class FruttecGet : public QObject
{
    Q_OBJECT
private:
    QSqlDatabase *db;
    bool initDb(QSqlDatabase &sqlTmp, int nDb, QString szThread);
    QTimer *tmrCheckSync;
    QThreadPool *poolSet;
    QThreadPool *poolProcess;
    FruttecGet *fruttecGet;
    QThread *threadGet;
    int nThreads;
    QMap<QString, QVariant> lstRows;
    int nRegPend;
    int nRegProc;
    QString szInstanciaDat;
public:
    explicit FruttecGet(QObject *parent = 0);

signals:
    void processRows(QMap<QString, QVariant> lstRows);
public slots:
    void runThread();
    void newConfig(QString szId, QMap<QString, QVariant> mapConfig);
private slots:
     void getSync();

};

#endif // FRUTTECGET_H
