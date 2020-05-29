#include "gbwriter.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTimer>
#include "def.h"
#include "util/str.h"
#include "util/f.h"

GbWriter::GbWriter(QObject *parent, logger* pLogger) : FileWriter(parent, pLogger)
{

}
QString GbWriter::gbOpen()
{
    QDir dir(QDir::tempPath());
    QString sSubDir = QString::number(QCoreApplication::applicationPid());
    if(!dir.mkdir(sSubDir))  {
        m_pLogger->err("GbWriter:GbOpen !mkdir: " + sSubDir);
    }

    m_sAbsFN = str::makeAbsFN(QDir::tempPath(), sSubDir);
    m_sAbsFN = str::makeAbsFN(m_sAbsFN, QString::number(QDateTime::currentMSecsSinceEpoch()) + ".htm");

    FileWriter::open(m_sAbsFN);
    return this->m_sAbsFN;
}
bool GbWriter::writeEndAndClose()
{
    this->writeCss();
    this->write("\n</body></html>");
    bool b = this->close();
    return b;

}
bool GbWriter::writeHead(QString sHtmlBlueprint)
{
    QString shtml(sHtmlBlueprint.toLower());
    int iBody = shtml.indexOf("<body");
    QString sHead = sHtmlBlueprint.left(iBody);
    sHead = sHead.replace("background: #","backgroundDeactivated: #");
    sHead = sHead.replace("font-size","font--size");
    //TODO: do i need <base href?
    this->write(sHead);
    return this->write("\n<body><link rel='stylesheet' href='" + str::getFN(__cssFN) + "'>");
}
bool GbWriter::writeCss()
{
    QString sCssFC = f::getFC(f::getResFn(__cssFN));
    QString sCssFN = str::makeAbsFN(str::getDir(m_sAbsFN), str::getFN(__cssFN));
    bool b = f::write(sCssFN, sCssFC);
    m_pLogger->log(this->getClassName() + ":writeCSS:" + sCssFN, b ? logger::LogLevel::INF : logger::LogLevel::WRN);
    return b;
}
bool GbWriter::close()
{
    return FileWriter::close();
}
