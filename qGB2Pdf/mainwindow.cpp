#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "def.h"

#include <QCloseEvent>
#include <QDirIterator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_pGbWorker(nullptr)
    , m_pTumbler(nullptr)
    , m_logger(nullptr, nullptr, this)
    , m_cfg(this)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //TODO: use splitter!
    /*{
        QList<int> sizes;
        sizes << 9; //top area
        sizes << 1; //bottom area (log wnd)
        this->ui->splitter->setSizes(sizes);
    }*/

    //TODO: log also onto statusbar
    this->m_logger.init(&this->m_cfg, this->ui->logWindow);

    QString sUrl(this->ui->eGbUrl->text());
    on_eGbUrl_textChanged(sUrl);
    if (sUrl.length() > 3)
        on_eGbUrl_returnPressed();
    connect(this->ui->webEngineView, SIGNAL(loadFinished(bool)), this, SLOT(on_loadFinished(bool)));
    connect(this->ui->webEngineView, SIGNAL(loadStarted()), this, SLOT(on_loadStarted()));
    connect(this->ui->webEngineView, SIGNAL(loadProgress(int)), this, SLOT(on_loadProgress(int)));
    connect(this->ui->webEngineView, SIGNAL(urlChanged(QUrl)), this, SLOT(on_urlChanged(QUrl)));
    void urlChanged(const QUrl&);

    this->ui->bRunning->setCheckable(true);
    this->ui->bRunning->setChecked(true);
    this->ui->bRunning->setani(":/res/boring.gif");
    this->ui->bRunning->setEnabled(false);

    //checkQrc();
    //m_pTumbler = new Tumbler(this, this->ui->webEngineView, &this->m_logger);
    //m_pTumbler->dwnld("pinayfarang");
}

void MainWindow::closeEvent(QCloseEvent *evt)
{
    if (!this->m_pGbWorker) {
       evt->accept();
    } else {
        this->m_logger.log("mainwindow: Working ... cannot close!", logger::LogLevel::WRN);
        evt->accept();
        //evt->ignore();    // TODO allow/disallow forced close
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_eGbUrl_returnPressed()
{
    if (m_pGbWorker) return;
    QString sUrl(this->ui->eGbUrl->text());
    this->ui->webEngineView->load(QUrl(sUrl));
    this->ui->bConvert->setEnabled(false);
}

void MainWindow::on_bConvert_clicked()
{
    m_pGbWorker = new GbWorker(this, this->ui->webEngineView, &this->m_logger);
    connect(this->m_pGbWorker, SIGNAL(scrapFinished(QString)), this, SLOT(scrapFinished(QString)));
    m_pGbWorker->startScrapingWithCurrentPage();
    this->ui->bConvert->setEnabled(false);
    this->ui->eGbUrl->setEnabled(true);
    this->ui->bRunning->setani(":/res/wait.gif");
    this->ui->bRunning->setEnabled(true);
}

void MainWindow::on_eGbUrl_textChanged(const QString &gbUrl)
{
    if (m_pGbWorker) return;
    //TODO: do it in the reactive way! (RxQt?)
    this->ui->bConvert->setEnabled(gbUrl.length() > 5 ? true : false);
}

void MainWindow::on_loadFinished(bool ok)
{
    if (!this->m_pGbWorker)
        m_logger.log("mainwindow: ok(" + QString::number(ok) + "): " + this->ui->webEngineView->url().toString(), ok ? logger::LogLevel::INF : logger::LogLevel::WRN);

    if (/*!ok || */this->m_pGbWorker) {
        this->ui->bConvert->setEnabled(false);
        return;
    }
    if (!this->m_pGbWorker)
    {
        //TODO: move this logic into GbWorker!?
        this->ui->webEngineView->page()->toHtml([this](QString html)
        {
            bool gbPage = html.indexOf(__pageContaner) > 9 && html.indexOf(__cardNext) > 9;
            this->ui->bConvert->setEnabled(gbPage);
            if (!gbPage) this->m_logger.log("mainwindow: URL not supported", logger::LogLevel::WRN);
        });
    }
}

void MainWindow::scrapFinished(QString sFN)
{
    disconnect(this->m_pGbWorker, SIGNAL(scrapFinished(QString)), this, SLOT(on_loadFinished(bool)));
    this->m_pGbWorker->deleteLater();
    this->m_pGbWorker = nullptr;

    this->ui->eGbUrl->setEnabled(true);
    QString sUrl(this->ui->eGbUrl->text());
    this->on_eGbUrl_textChanged(sUrl);

    this->ui->tabWidget->setCurrentIndex(1);

    QUrl urlFN = QUrl::fromLocalFile(sFN);  //required pattern; file:///C:/temp/12904/1588668531602.htm
    this->m_logger.log("mainwindow: FINISH: fn: " + sFN, logger::LogLevel::INF);
    this->m_logger.log("mainwindow: FINISH: url:" + urlFN.toString(), logger::LogLevel::INF);

    //TODO: load animation while loading huge offline file!
    this->ui->webEngineViewOffline->setUrl(urlFN);
    this->ui->bRunning->setani(":/res/boring.gif");
    this->ui->bRunning->setEnabled(false);
}

void MainWindow::on_loadStarted()
{
    //this->m_logger.log("mainwindow: Loading... ", logger::LogLevel::INF);
}

void MainWindow::on_loadProgress(int progress)
{
    if (progress > 99)
    {
        //TODO: instead of stopping, setup timer and stop on expiring (or abandon timer if loadFinished faster)
        this->ui->webEngineView->stop();
    }
    //this->m_logger.log("mainwindow: Loading... " + QString::number(progress) + "%", logger::LogLevel::INF);
}

void MainWindow::on_urlChanged(QUrl url)
{
    this->ui->eGbUrl->setText(url.toString());
}

void MainWindow::checkQrc()
{
    QDirIterator it(":/res", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        qDebug() << it.next();
    }

    QString sRes(":/res/gb2pdf.js");
    //QString sRes(":/res/gb.png");

    QFile file(sRes);
    if(!file.open(QIODevice::ReadOnly)) {
        //should never happen, as file is in resources
        this->m_logger.err("mainwindows:checkQrc "+sRes+" file not opened");
        return;
    }
    QString s = file.readAll();
    file.close();
    this->m_logger.log("mainwindows:checkQrc read: " + QString::number(s.length()), s.length() > 0 ? logger::LogLevel::INF : logger::LogLevel::ERR);
}

//TODO: no [x] when working!
//TODO: disable edit, show wait icon animation when working!
