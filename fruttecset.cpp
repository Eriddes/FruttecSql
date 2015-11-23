#include "fruttecset.h"

FruttecSet::FruttecSet()
{
    nTimeout = 3000;
}
void FruttecSet::run()
{
    //int nThread = (int)QThread::currentThreadId();
    emit newConfig(rowSet["ID"].toString(),getConfig());
}
QMap<QString, QVariant> FruttecSet::getConfig()
{
    QMap<QString, QVariant> mapOutput;
    QStringList lstParams;
    lstParams.append("GET_CONFIG");                  //1 - ACTION
    lstParams.append(QString::number(nTimeout));                        //2 - TIMEOUT
    lstParams.append(rowSet["ID"].toString());       //3 - ID_I

    QString szResult = remoteFuncV7("127.0.0.1","6021","PROCESOS", "SQL_SINC", "fun", "1234", lstParams);
    if(szResult!="NULL" && szResult.length()>2)
    {
        //qDebug()<< szResult;
        Json *json = new Json();
        mapOutput = json->decode(szResult);
    }
    return mapOutput;
}
QString FruttecSet::remoteFuncV7(QString szHost, QString szPort, QString szInstance, QString szFunction, QString szUser, QString szPass, QStringList lstParams)
{
    QString szOutput;
    QString szTrama="";
    QByteArray data;
    QTcpSocket *socket;
    socket = new QTcpSocket();
    socket->open(QAbstractSocket::ReadWrite);
    socket->connectToHost(szHost,szPort.toInt());
    if (socket->waitForConnected ())
    {
        szTrama = "VR2e "+szInstance+" "+szFunction+" "+szUser+" "+hash(szPass,2);
        szTrama.append(QString("%1").arg((char)10)+QString("%1").arg((char)0)+QString("%1").arg((char)0)+QString("%1").arg((char)0)+QString("%1").arg((char)8));
        for(int i=0; i < lstParams.size(); i++)
        {
            szTrama.append(param(lstParams.at(i)));
        }
        for(int i=lstParams.size(); i < 8; i++)
        {
            szTrama.append(param(""));
        }
        socket->write(szTrama.toAscii());
        QTime t;
        t.start();
        socket->waitForReadyRead(abs(nTimeout-1000));
        data = socket->read(7);
        if(data.length() > 6)
        {//Respuesta OK
            QString szLen = data.toHex().mid(6,8);
            bool ok;
            uint len = szLen.toUInt(&ok,16);
            while((data.length() < (len+3)) || (t.elapsed() < nTimeout))
            {
                socket->waitForReadyRead(500);
                data += socket->readAll();
            }
            data = data.mid(13,data.length()-13);
            data = data.replace('\0',"");
            szOutput = (QString)data;
        }
        socket->close();
    }
    return szOutput;
}
QString FruttecSet::param(QString szPar){
    QString szResponse;
    QString szData;
    QString len1;
    QString len1Ascii;
    QString len2;
    QString len2Ascii;
    QString tmp;
    int conta;

    for(int i=0; i<szPar.length();i++ )
    {
        szData.append(QString("%1").arg((char)0)+szPar.at(i));
    }
    len1 = len1.setNum(szData.length()+5, 16);
    len1 = len1.rightJustified(8, '0');
    conta=0;
    tmp="";
    for(int i=0; i<len1.length();i++ ){
        tmp=tmp+len1.at(i);
        conta++;
        if(conta==2)
        {
            conta=0;
            if(tmp=="00")
                len1Ascii.append(tmp.toInt());
            else
                len1Ascii.append(QByteArray::fromHex(tmp.toUtf8()));
            tmp="";
        }
    }
    len2 = len2.setNum(szData.length(), 16);
    len2 = len2.rightJustified(8, '0');
    conta=0;
    tmp="";
    for(int i=0; i<len2.length();i++ ){
        tmp=tmp+len2.at(i);
        conta++;
        if(conta==2)
        {
            conta=0;
            if(tmp=="00")
                len2Ascii.append(tmp.toInt());
            else
                len2Ascii.append(QByteArray::fromHex(tmp.toUtf8()));
            tmp="";
        }
    }
    szResponse.append(len1Ascii+QString("%1").arg((char)1)+len2Ascii+szData);
    return szResponse;
}
QString FruttecSet::hash(QString datos, int modo)
{
    QCryptographicHash *hs;
    switch(modo)
    {
        case 0:
            hs = new QCryptographicHash(QCryptographicHash::Md4);
            break;

        case 1:
            hs = new QCryptographicHash(QCryptographicHash::Md5);
            break;

        case 2:
            hs = new QCryptographicHash(QCryptographicHash::Sha1);
            break;

        default:
            break;
    }

    hs->addData(datos.toUtf8());
    QByteArray aux = hs->result().toHex();

    delete hs;
    return QString(aux);
}
