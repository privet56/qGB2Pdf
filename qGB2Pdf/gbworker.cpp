#include "gbworker.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include "def.h"
#include "util/str.h"

GbWorker::GbWorker(QObject *parent, QWebEngineView* pWebEngineView, logger* pLogger) : QObject(parent)
  , m_pLogger(pLogger)
  , m_pWebEngineView(pWebEngineView)
  , m_iPage(9)
{

}
void GbWorker::startScrapingWithCurrentPage()
{
    m_iPage = 0;
    connect(this->m_pWebEngineView, SIGNAL(loadFinished(bool)), this, SLOT(on_loadFinished(bool)));

    {   // fill m_sgb2pdf_js
        QFile file(":/res/gb2pdf.js");
        if(!file.open(QIODevice::ReadOnly)) {
            //should never happen, as file is in resources
            m_pLogger->err(":/res/gb2pdf.js file not opened");
        }
        else
        {
            m_sgb2pdf_js = file.readAll();
        }
        file.close();
    }
    {   //open temporary file
        QDir dir(QDir::tempPath());
        QString sSubDir = QString::number(QCoreApplication::applicationPid());
        if(!dir.mkdir(sSubDir))  {
            m_pLogger->err("!mkdir: " + sSubDir);
        }
        m_sScrapedFN = str::makeAbsFN(QDir::tempPath(), sSubDir);
        m_sScrapedFN = str::makeAbsFN(m_sScrapedFN, QString::number(QDateTime::currentMSecsSinceEpoch()) + ".htm");
        m_fScrapedContent.setFileName(m_sScrapedFN);
        if(!m_fScrapedContent.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            m_pLogger->err("!open: " + m_sScrapedFN);
        }
        m_sScrapedContent.setDevice(&m_fScrapedContent);
    }

    this->m_pWebEngineView->page()->toHtml([this](QString sHtml)
    {
        QString shtml(sHtml.toLower());
        int iBody = shtml.indexOf("<body");
        QString sHead = sHtml.left(iBody);
        m_sScrapedContent << sHead;

        on_loadFinished(true);
    });
}

void GbWorker::endScraping()
{
    this->m_sScrapedContent.flush();
    this->m_fScrapedContent.close();
    disconnect(this->m_pWebEngineView, SIGNAL(loadFinished(bool)), this, SLOT(on_loadFinished(bool)));
    emit scrapFinished(this->m_sScrapedFN);
}

void GbWorker::on_loadFinished(bool ok)
{
    if (!ok) {
        m_pLogger->wrn("!ok: " + this->m_pWebEngineView->url().toString());
        this->endScraping();
        return;
    }

    m_iPage++;
    this->m_pWebEngineView->page()->runJavaScript(m_sgb2pdf_js);

    //TODO: def js function name
    this->m_pWebEngineView->page()->runJavaScript("getPageContainer('" + __pageContaner + "');", [this](const QVariant &v) {
        QString pageContainer = v.toString();
        m_sScrapedContent << "\n<!-- page-begin ("  + QString::number(m_iPage) + ") -->\n";
        m_sScrapedContent << pageContainer;
        m_sScrapedContent << "\n<!-- page-end ("  + QString::number(m_iPage) + ") -->\n";

        m_pLogger->inf("scraping page" + QString::number(m_iPage) + ", " + this->m_pWebEngineView->url().toString());

        /*TODO: remove this code
        if (m_iPage > 3)
        {
            m_pLogger->wrn("breaking: " + this->m_pWebEngineView->url().toString());
            this->endScraping();
            return;
        }
        else*/
        {
            this->clickNextPage();
        }
    });
}

void GbWorker::clickNextPage()
{
    //TODO: def js function name
    this->m_pWebEngineView->page()->runJavaScript("clickCardNext('" + __cardNext + "');", [this](const QVariant &v) {
        bool clicked = v.toBool();
        if (!clicked)
        {
            m_pLogger->inf("end scraping: " + this->m_pWebEngineView->url().toString());
            this->endScraping();
            return;
        }
    });
}
