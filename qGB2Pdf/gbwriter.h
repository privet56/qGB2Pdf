#ifndef GBWRITER_H
#define GBWRITER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include "util/cfg.h"
#include "util/logger.h"

class GbWriter : public QObject
{
    Q_OBJECT
public:
    explicit GbWriter(QObject *parent, logger* pLogger);
public:
    QString gbOpen();
    QString getAbsFN();
    bool write(QString s);
    bool close();

protected:
    logger* m_pLogger;
protected:
    QFile m_File;
    QString m_sAbsFN;
    QTextStream m_stream;

signals:

};

#endif // GBWRITER_H
