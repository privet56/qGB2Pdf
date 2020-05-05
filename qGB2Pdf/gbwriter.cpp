#include "gbwriter.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTimer>
#include "def.h"
#include "util/str.h"

GbWriter::GbWriter(QObject *parent, logger* pLogger) : QObject(parent), m_pLogger(pLogger)
{

}
QString GbWriter::gbOpen()
{
    QDir dir(QDir::tempPath());
    QString sSubDir = QString::number(QCoreApplication::applicationPid());
    if(!dir.mkdir(sSubDir))  {
        m_pLogger->err("gbWriter: !mkdir: " + sSubDir);
    }
    m_sAbsFN = str::makeAbsFN(QDir::tempPath(), sSubDir);
    m_sAbsFN = str::makeAbsFN(m_sAbsFN, QString::number(QDateTime::currentMSecsSinceEpoch()) + ".htm");
    m_File.setFileName(m_sAbsFN);
    if(!m_File.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        m_pLogger->err("gbWriter: !open: " + m_sAbsFN);
    }
    m_stream.setDevice(&m_File);
    return this->m_sAbsFN;
}
QString GbWriter::getAbsFN()
{
    return this->m_sAbsFN;
}
bool GbWriter::write(QString s)
{
    if(!this->m_File.isOpen())
    {
        m_pLogger->err("gbWriter:write: !open: " + m_sAbsFN);
        return false;
    }
    m_stream << s;
    return true;
}
bool GbWriter::close()
{
    if(!this->m_File.isOpen())
    {
        m_pLogger->err("gbWriter:close: !open: " + m_sAbsFN);
        return false;
    }

    this->m_File.flush();
    this->m_File.close();
    return true;
}
