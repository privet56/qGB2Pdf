#ifndef GBWRITER_H
#define GBWRITER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include "util/cfg.h"
#include "util/logger.h"
#include "filewriter.h"

class GbWriter : public FileWriter
{
    Q_OBJECT
public:
    explicit GbWriter(QObject *parent, logger* pLogger);
public:
    QString gbOpen();
    virtual bool close();

    bool writeHead(QString sHtmlBlueprint);
    bool writeEndAndClose();

protected:
    bool writeCss();
protected:
    QFile m_File;
    QString m_sAbsFN;
    QTextStream m_stream;

signals:

};

#endif // GBWRITER_H
