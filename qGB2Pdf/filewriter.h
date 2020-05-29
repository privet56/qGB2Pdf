#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include "util/cfg.h"
#include "util/logger.h"

class FileWriter : public QObject
{
    Q_OBJECT
public:
    explicit FileWriter(QObject *parent, logger* pLogger);

public:
    bool open(QString& sAbsFN);
    QString getAbsFN();
    bool write(QString s);
    virtual bool close();

    QString getClassName();

public:
    logger* m_pLogger;
protected:
    QFile m_File;
    QString m_sAbsFN;
    QTextStream m_stream;

signals:

};

#endif // FILEWRITER_H
