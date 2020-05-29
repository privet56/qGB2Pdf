#include "filewriter.h"
#include "util/str.h"

FileWriter::FileWriter(QObject *parent, logger* pLogger) : QObject(parent), m_pLogger(pLogger)
{

}
bool FileWriter::open(QString& sAbsFN)
{
    this->m_sAbsFN = sAbsFN;
    m_File.setFileName(m_sAbsFN);
    if(!m_File.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        m_pLogger->err("FileWriter: !open: " + m_sAbsFN);
        return false;
    }
    m_stream.setDevice(&m_File);
    return true;
}
QString FileWriter::getAbsFN()
{
    return this->m_sAbsFN;
}
bool FileWriter::write(QString s)
{
    if(!this->m_File.isOpen())
    {
        m_pLogger->err("FileWriter:write: !open: " + m_sAbsFN);
        return false;
    }
    m_stream << s;
    return true;
}
bool FileWriter::close()
{
    if(!this->m_File.isOpen())
    {
        m_pLogger->err("FileWriter:close: !open: " + m_sAbsFN);
        return false;
    }

    this->m_File.flush();
    this->m_File.close();
    return true;
}
QString FileWriter::getClassName()
{
    static QString __sClassName("");
    if(str::isempty(__sClassName, false))
    {
        __sClassName = QString::fromLatin1(this->metaObject()->className());
    }
    return __sClassName;
}
