#ifndef W_H
#define W_H

#include <QWidget>
#include "logger.h"

class w : public QWidget
{
    Q_OBJECT
public:
    explicit w(QWidget *parent = 0);
    static QString getOpenFileName(QWidget* parent, QString sTitle, QString sPattern);
    static void killSubProcesses(qint64 pid, logger* pLogger, int iRecDepth=0);

signals:

public slots:
};

#endif // W_H
