#include "gbworker.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTimer>
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
    connect(this->m_pWebEngineView, SIGNAL(loadStarted()), this, SLOT(on_loadStarted()));
    connect(this->m_pWebEngineView, SIGNAL(loadProgress(int)), this, SLOT(on_loadProgress(int)));


    {   // fill m_sgb2pdf_js
        QFile file(":/res/gb2pdf.js");
        if(!file.open(QIODevice::ReadOnly)) {
            //should never happen, as file is in resources
            m_pLogger->err("gbWorker: :/res/gb2pdf.js file not opened");
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
            m_pLogger->err("gbWorker: !mkdir: " + sSubDir);
        }
        m_sScrapedFN = str::makeAbsFN(QDir::tempPath(), sSubDir);
        m_sScrapedFN = str::makeAbsFN(m_sScrapedFN, QString::number(QDateTime::currentMSecsSinceEpoch()) + ".htm");
        m_fScrapedContent.setFileName(m_sScrapedFN);
        if(!m_fScrapedContent.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            m_pLogger->err("gbWorker: !open: " + m_sScrapedFN);
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
    //TODO: convert local collected html to PDF!
}

void GbWorker::on_loadFinished(bool ok)
{
    if (!ok) {
        m_pLogger->wrn("gbWorker: !ok: " + this->m_pWebEngineView->url().toString());
        //TODO: progress>99 -> stop  -> should be ok
        //this->endScraping();
        //return;
    }

    m_iPage++;
    this->m_pWebEngineView->page()->runJavaScript(m_sgb2pdf_js);

    m_pLogger->inf("gbWorker: ok & js injected: " + this->m_pWebEngineView->url().toString());

    //TODO: def js function name
    //TODO: getPageContainer should get the innerHTML of the first child!
    this->m_pWebEngineView->page()->runJavaScript("getPageContainer('" + __pageContaner + "');", [this](const QVariant &v) {
        QString pageContainer = v.toString();
        m_sScrapedContent << "\n<!-- page-begin ("  + QString::number(m_iPage) + ") -->\n";
        m_sScrapedContent << pageContainer;
        m_sScrapedContent << "\n<!-- page-end ("  + QString::number(m_iPage) + ") -->\n";

        m_pLogger->inf("gbWorker: scraping page" + QString::number(m_iPage) + ", " + this->m_pWebEngineView->url().toString());

        /*TODO: remove this code
        if (m_iPage > 3)
        {
            m_pLogger->wrn("gbWorker: breaking: " + this->m_pWebEngineView->url().toString());
            this->endScraping();
            return;
        }
        else*/
        {
            QTimer::singleShot(999, this, SLOT(clickNextPage()));
            //this->clickNextPage();
        }
    });
}

void GbWorker::clickNextPage()
{
    m_pLogger->inf("gbWorker: trying to click next: " + this->m_pWebEngineView->url().toString());
    //TODO: def js function name
    this->m_pWebEngineView->page()->runJavaScript("clickCardNext('" + __cardNext + "');", [this](const QVariant &v) {
        bool clicked = v.toBool();
        if (!clicked)
        {
            m_pLogger->inf("gbWorker:clickNextPage -> no next link -> end scraping: " + this->m_pWebEngineView->url().toString());
            this->endScraping();
            return;
        }
        else
        {
            m_pLogger->inf("gbWorker: next link clicked: " + this->m_pWebEngineView->url().toString());
        }
    });
}
void GbWorker::on_loadStarted()
{
    m_pLogger->log("gbWorker: Loading... ", logger::LogLevel::INF);
}

void GbWorker::on_loadProgress(int progress)
{
    if (progress > 99)
        this->m_pWebEngineView->stop();
    m_pLogger->log("gbWorker: Loading... " + QString::number(progress) + "%", logger::LogLevel::INF);
}
