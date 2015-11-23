#ifndef FRUTTECSET_H
#define FRUTTECSET_H

#include <QObject>
#include <QRunnable>
#include <QThread>
#include <QDebug>
#include <QTcpSocket>
#include <QCryptographicHash>
#include <QStringList>
#include <QTime>
#include "json.h"

class FruttecSet : public QObject, public QRunnable
{
    Q_OBJECT
private:
    QString remoteFuncV7(QString szHost, QString szPort, QString szInstance, QString szFunction, QString szUser, QString szPass, QStringList lstParams);
    QString hash(QString datos, int modo);
    QString param(QString szPar);
    QMap<QString, QVariant> getConfig();
    int nTimeout;
public:
    explicit FruttecSet();
    QMap<QString, QVariant> rowSet;
protected:
    void run();

signals:
    void newConfig(QString szId, QMap<QString, QVariant> mapRow);
public slots:
};

#endif // FRUTTECSET_H
