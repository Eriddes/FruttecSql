#ifndef FRUTTECPROCESS_H
#define FRUTTECPROCESS_H
#include <QObject>
#include <QRunnable>

class FruttecProcess : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit FruttecProcess(QObject *parent = 0);

signals:

public slots:
};

#endif // FRUTTECPROCESS_H
