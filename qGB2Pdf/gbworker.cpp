#include "gbworker.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QTimer>
#include "def.h"
#include "util/str.h"

GbWorker::GbWorker(QObject *parent, QWebEngineView* pWebEngineView, logger* pLogger) : QObject(parent)
  , m_pLogger(pLogger)
  , m_pWebEngineView(pWebEngineView)
  , m_gbWriter(this, pLogger)
  , m_loadFinishedTimer(this)
  , m_iPage(0)
{
    {   //setup timer
        m_loadFinishedTimer.setInterval(1999);
        m_loadFinishedTimer.setSingleShot(true);
        connect(&m_loadFinishedTimer, &QTimer::timeout, [this]() {
            m_loadFinishedTimer.stop();
            m_pLogger->inf("gbWorker:m_loadFinishedTimer timeout -> on_loadFinished(true)");
            this->on_loadFinished(true);
        });
        m_loadFinishedTimer.stop();
    }
}
void GbWorker::startScrapingWithCurrentPage()
{
    m_iPage = 0;
    connect(this->m_pWebEngineView, SIGNAL(loadFinished(bool)), this, SLOT(on_loadFinished(bool)));
    connect(this->m_pWebEngineView, SIGNAL(loadStarted()),      this, SLOT(on_loadStarted()));
    connect(this->m_pWebEngineView, SIGNAL(loadProgress(int)),  this, SLOT(on_loadProgress(int)));

    {   // fill m_sgb2pdf_js
        QFile file(":/res/gb2pdf.js");
        if(!file.open(QIODevice::ReadOnly)) {
            //should never happen, as file is in resources
            m_pLogger->err("gbWorker: :/res/gb2pdf.js file not opened");
            return;
        }
        else
        {
            m_sgb2pdf_js = file.readAll();
            file.close();
            if(str::isempty(m_sgb2pdf_js, true))
            {
                m_pLogger->err("gbWorker:startScrapingWithCurrentPage m_sgb2pdf_js:\n" + m_sgb2pdf_js + "\n-----------------");
                return;
            }
        }
    }

    m_gbWriter.gbOpen();

    this->m_pWebEngineView->page()->toHtml([this](QString sHtml)
    {
        QString shtml(sHtml.toLower());
        int iBody = shtml.indexOf("<body");
        QString sHead = sHtml.left(iBody);
        sHead = sHead.replace("background: #","backgroundDeactivated: #");
        sHead = sHead.replace("font-size","font--size");
        //TODO: do i need <base href?
        m_gbWriter.write(sHead);
        m_gbWriter.write("\n<body><link rel='stylesheet' href='" + str::getFN(__cssFN) + "'>");

        on_loadFinished(true);
    });
}

void GbWorker::endScraping()
{
    m_loadFinishedTimer.stop();

    m_gbWriter.writeCss();
    m_gbWriter.write("\n</body></html>");
    m_gbWriter.close();
    disconnect(this->m_pWebEngineView, SIGNAL(loadFinished(bool)), this, SLOT(on_loadFinished(bool)));
    disconnect(this->m_pWebEngineView, SIGNAL(loadStarted()),      this, SLOT(on_loadStarted()));
    disconnect(this->m_pWebEngineView, SIGNAL(loadProgress(int)),  this, SLOT(on_loadProgress(int)));

    emit scrapFinished(m_gbWriter.getAbsFN());

    {
        QString sHtmAbsFn = m_gbWriter.getAbsFN();
        QString sPdfAbsFn = sHtmAbsFn + ".pdf";
        QString sExe = str::makeAbsFN(qApp->applicationDirPath(), "bin");
        sExe = str::makeAbsFN(sExe, "wkhtmltopdf.exe");
        QStringList params;
        params << sHtmAbsFn << sPdfAbsFn;
        //TODO: check if works with space's in the paths & params!
        //TODO: catch stdout & stderr?
        //TODO: check first if exe & html exist
        int iExitCode = QProcess::execute(sExe, params);
        m_pLogger->log("gbWorker:endScraping iExitCode:" + QString::number(iExitCode)+" -> " + sPdfAbsFn, iExitCode == 0 ? logger::LogLevel::INF : logger::LogLevel::WRN);
        QDesktopServices::openUrl(QUrl::fromLocalFile(sPdfAbsFn));
    }
    //TODO: convert local collected html to PDF! with wkhtmlto... lib or dll.!?
    //TODO: show generated PDF in a 3. tab (with webengine-pdf-dll addon?)
}

void GbWorker::on_loadFinished(bool ok)
{
    m_loadFinishedTimer.stop();
    this->m_pWebEngineView->stop();

    m_pLogger->log("gbWorker: "+ QString(ok ? "" : "!") +"ok: " + this->m_pWebEngineView->url().toString(), !ok ? logger::LogLevel::WRN : logger::LogLevel::INF);

    /*  WE CANNOT TRUST this flag!
    if (!ok) {
        this->endScraping();
        return;
    }*/

    m_iPage++;
    this->m_pWebEngineView->page()->runJavaScript(m_sgb2pdf_js, [this](const QVariant &v) {
        if (v.isNull() || !v.isValid())
        {
            qDebug() << v;
        }
        this->on_JsInjected();
    });
}
void GbWorker::on_JsInjected()
{
    //m_pLogger->inf("gbWorker: js injected -> getPageContainer! url:" + this->m_pWebEngineView->url().toString());

    //TODO: def js function name
    this->m_pWebEngineView->page()->runJavaScript("getPageContainer('" + __pageContaner + "');", [this](const QVariant &v) {
        QString pageContainer = v.toString();
        m_gbWriter.write("\n<!-- page-begin ("  + QString::number(m_iPage) + ") -->\n");
        m_gbWriter.write(pageContainer);
        m_gbWriter.write("\n<!-- page-end ("  + QString::number(m_iPage) + ") -->\n");
        m_gbWriter.write("\n<div style='page-break-after:always;'>&nbsp;</div>");
        if (pageContainer.length() < 3)
        {
            qDebug() << v;
            m_pLogger->wrn("gbWorker: scraping empty? page" + QString::number(m_iPage) + ", " + this->m_pWebEngineView->url().toString()+" pageContainer:'"+pageContainer+"'");
        }
        else
        {
            if (v.isNull() || !v.isValid())
            {
                qDebug() << v;
            }
            m_pLogger->inf("gbWorker: scraping page " + QString::number(m_iPage) + ", " + this->m_pWebEngineView->url().toString());
        }

        /*TODO: remove this code (better: control by cfg)
        if (m_iPage > 3)
        {
            m_pLogger->wrn("gbWorker: breaking: " + this->m_pWebEngineView->url().toString());
            this->endScraping();
            return;
        }
        else*/
        {
            QTimer::singleShot(999/*ms*/, this, SLOT(clickNextPage()));
        }
    });
}

void GbWorker::clickNextPage()
{
    //m_pLogger->inf("gbWorker: trying to click next: " + this->m_pWebEngineView->url().toString());
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
            //m_pLogger->inf("gbWorker: next link clicked: " + this->m_pWebEngineView->url().toString());
        }
    });
}
void GbWorker::on_loadStarted()
{
    //m_pLogger->log("gbWorker: Load Started! ", logger::LogLevel::INF);
}

void GbWorker::on_loadProgress(int progress)
{
    //TODO: call directyle here onLoadFinished(true) & ->stop there within the function!?
    if (progress > 99)
    {
        //TODO: why is called on_loadProgress(100) twice?
        m_pLogger->log("gbWorker: Loaded! ... " + QString::number(progress) + "%", logger::LogLevel::INF);
        m_loadFinishedTimer.stop();
        m_loadFinishedTimer.start();
    }
    else
    {
        //m_pLogger->log("gbWorker: Loading... " + QString::number(progress) + "%", logger::LogLevel::INF);
    }
}
