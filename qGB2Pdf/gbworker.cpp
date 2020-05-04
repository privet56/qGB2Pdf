#include "gbworker.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>

GbWorker::GbWorker(QObject *parent, QWebEngineView* pWebEngineView) : QObject(parent), m_pWebEngineView(pWebEngineView)
{
    connect(this->m_pWebEngineView, SIGNAL(loadFinished(bool)), this, SLOT(on_loadFinished(bool)));

    {   // fill m_sgb2pdf_js
        QFile file(":/res/gb2pdf.js");
        if(!file.open(QIODevice::ReadOnly)) {
            qDebug()<<"ERR: :/res/gb2pdf.js file not opened" << endl;
        }
        else
        {
            m_sgb2pdf_js = file.readAll();
        }
        file.close();
    }
    {
        QDir dir(QDir::tempPath());
        QString sSubDir = QString::number(QCoreApplication::applicationPid());
        if(!dir.mkdir(sSubDir))  {
            qDebug()<<"ERR: !mkdir: " << sSubDir << endl;
        }
        m_sScrapedFN = QDir::tempPath() + QDir::separator() + sSubDir + QDir::separator() + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".htm";
        if(!m_fScrapedContent.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            qDebug()<<"ERR: !open: " << m_sScrapedFN << endl;
        }
    }
}

void GbWorker::on_loadFinished(bool ok)
{
    Q_UNUSED(ok);

    this->m_pWebEngineView->page()->runJavaScript(m_sgb2pdf_js);

    this->m_pWebEngineView->page()->runJavaScript("getPageContainer('--pageContainer-');", [this](const QVariant &v) {
        QString pageContainer = v.toString();
        QTextStream stream(&this->m_fScrapedContent);
        stream << "\n<!-- page-begin -->\n";
        stream << pageContainer;
        stream << "\n<!-- page-end -->\n";
    });

}
