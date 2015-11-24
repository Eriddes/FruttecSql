#include "fruttecget.h"


//Es es un comentario
FruttecGet::FruttecGet(QObject *parent) : QObject(parent){

    db = new QSqlDatabase();
    nThreads = 100;
    poolSet = new QThreadPool(this);
    poolSet->setMaxThreadCount(nThreads);

    poolProcess = new QThreadPool(this);
    poolProcess->setMaxThreadCount(nThreads);

    szInstanciaDat = "eriddesGestionDat";

}
void FruttecGet::runThread()
{
    db = new QSqlDatabase();
    tmrCheckSync = new QTimer();
    connect(tmrCheckSync, SIGNAL(timeout()), this, SLOT(getSync()));
    tmrCheckSync->setInterval(500);
    tmrCheckSync->start();
}
void FruttecGet::getSync()
{
    tmrCheckSync->stop();
    QWaitCondition waitCondition;
    QMutex mutex;
    qint64 nTimIni = QDateTime::currentMSecsSinceEpoch();
    if(initDb(*db,0, "GET"))
     {
        QSqlQuery query(*db);
        bool bExec = false;
        QString szQuery = "SELECT ID, REF, COD, EST, TIP, ORD, KEY_CAMP FROM gd21_util_dat#BD_LOG_UTI where est='0'";
        bExec = query.exec(szQuery);
        if(bExec)
        {
            QString szFieldName;
            QSqlRecord rec;
            rec = query.record();
            lstRows.clear();
            //Recorremos los registros obtenidos
            nRegPend = 0;
            nRegProc= 0;
            while(query.next())
            {
                QMap<QString, QVariant> mapLinConfig;
                int nFields = rec.count();
                for(int i=0; i<nFields; i++)
                {
                    szFieldName = rec.fieldName(i).toUpper();
                    mapLinConfig[szFieldName] = query.record().value(i).toString();
                }
                nRegPend++;

                //Iniciamos el envio remoto para obtener la configuracion de tabla
                FruttecSet *fruttecSet = new FruttecSet();
                fruttecSet->autoDelete();
                fruttecSet->rowSet = mapLinConfig;
                connect(fruttecSet, SIGNAL(newConfig(QString, QMap<QString,QVariant>)), this, SLOT(newConfig(QString, QMap<QString,QVariant>)));
                poolSet->start(fruttecSet);

                //La key es el id del registro de cola de sincronizaciones
                lstRows[mapLinConfig["ID"].toString()] = mapLinConfig;

                //Esperamos 150 mls para no saturar velneo
                waitCondition.wait(&mutex,150);
            }
            QTime t;
            t.start();
            //Timeout de 1000mls por registro
            while(t.elapsed() < (2000*nRegPend))
            {
               //Entretenemos hasta finalizar
                QCoreApplication::processEvents();
                if(nRegProc==nRegPend)
                    break;
            }
            //qDebug() << "Tiempo de proceso:" << (QDateTime::currentMSecsSinceEpoch() - nTimIni) << " mls" << " nReg:" << nRegPend;
            lstRows.clear();
            nRegPend = 0;
            nRegProc= 0;
            QCoreApplication::processEvents();

        }
     }
    //tmrCheckSync->start();
}
void FruttecGet::newConfig(QString szId, QMap<QString,QVariant> mapConfig)
{
    qDebug() << "****************************";
    if(lstRows.contains(szId))
    {

        QString szKey, szWhere;
        QSqlQuery query(*db);
        QMap<QString, QVariant> mapRowTmp;
        mapRowTmp = lstRows[szId].toMap();
        mapRowTmp["OK_CFG"] = false;
        szKey = mapRowTmp["KEY_CAMP"].toString();
        if(szKey.length()>0)
        {
            qDebug() << "CONFIG OK:" << szId;
            QString szTableName = mapRowTmp["REF"].toString().replace("/","#");
            if(szTableName.contains(szInstanciaDat)){
                szTableName =  szTableName.replace(szInstanciaDat,"");
                szTableName =  szTableName.replace("#","");
            }

            if(mapConfig[szKey.toLower()].toMap()["type"].toInt()!=6)
                szWhere = "'"+szId+"'";
            else
                szWhere = szId;

            QString szQuery = "SELECT * FROM "+szTableName+" where "+ szKey+"="+szWhere+"";
            bool bExec = query.exec(szQuery);
            if(bExec)
            {
                QString szFieldName;
                QSqlRecord rec;
                rec = query.record();
                while(query.next())
                {
                    QMap<QString, QVariant> mapLinConfig;
                    int nFields = rec.count();
                    for(int i=0; i<nFields; i++)
                    {
                        szFieldName = rec.fieldName(i).toUpper();
                        mapLinConfig[szFieldName] = query.record().value(i).toString();
                    }
                    mapRowTmp["DATA"] = mapLinConfig;
                }
                mapRowTmp["OK_CFG"] = true;
            }
        }
        nRegProc++;
        mapRowTmp["TABLE"] = mapConfig;
        lstRows[szId] = mapRowTmp;
        emit processRows(mapRowTmp);
    }
}
bool FruttecGet::initDb(QSqlDatabase &sqlTmp, int nDb, QString  szThread)
{
    int nGlobalPos = 0;
    bool bStarted = false;
    nGlobalPos = nDb;

    QString szConName = szThread+"_"+QString::number(nDb);
    sqlTmp = QSqlDatabase::database(szConName);
    if(!sqlTmp.isValid())
    {
        sqlTmp = QSqlDatabase::addDatabase("QODBC",szConName);
        sqlTmp.setDatabaseName("Fruttec");
        sqlTmp.setUserName("sql");
    }
    if(!sqlTmp.isOpen())
    {
        if(sqlTmp.open())
        {
            qDebug()<< "        +DB ABIERTA " << szConName;
            bStarted=true;
        }
        else
        {
            qDebug()<< "        +DB NO SE PUEDE ABRIR " << szConName <<  sqlTmp.lastError();
            bStarted=false;
        }
    }
    else
    {
        bStarted=true;
    }
    return bStarted;
}
