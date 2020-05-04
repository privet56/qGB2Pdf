#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "def.h"

#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_pGbWorker(nullptr)
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
}

void MainWindow::on_eGbUrl_textChanged(const QString &gbUrl)
{
    if (m_pGbWorker) return;
    this->ui->bConvert->setEnabled(gbUrl.length() > 5 ? true : false);
}

void MainWindow::on_loadFinished(bool ok)
{
    m_logger.inf("mainwindow: ok(" + QString::number(ok) + "): " + this->ui->webEngineView->url().toString());

    //TODO: progress>99 -> stop  -> should be ok
    if (/*!ok || */this->m_pGbWorker) {
        this->ui->bConvert->setEnabled(false);
        if (!ok)
            this->m_logger.log("mainwindow: load !ok: " + this->ui->webEngineView->url().toString(), logger::LogLevel::WRN);
        return;
    }
    //TODO: move this logic into GbWorker!?
    this->ui->webEngineView->page()->toHtml([this](QString html)
    {
        bool gbPage = html.indexOf(__pageContaner) > 9 && html.indexOf(__cardNext) > 9;
        this->ui->bConvert->setEnabled(gbPage);
        if (!gbPage) this->m_logger.log("mainwindow: URL not supported", logger::LogLevel::WRN);
    });
}

void MainWindow::scrapFinished(QString sFN)
{
    this->m_logger.log("mainwindow: FINISH: " + sFN, logger::LogLevel::INF);
    disconnect(this->m_pGbWorker, SIGNAL(scrapFinished(QString)), this, SLOT(on_loadFinished(bool)));
    this->m_pGbWorker->deleteLater();
    this->m_pGbWorker = nullptr;

    this->ui->eGbUrl->setEnabled(true);
    QString sUrl(this->ui->eGbUrl->text());
    this->on_eGbUrl_textChanged(sUrl);

    this->ui->tabWidget->setCurrentIndex(1);
    this->ui->webEngineViewOffline->setUrl(QUrl(sFN));
}

void MainWindow::on_loadStarted()
{
    //this->m_logger.log("mainwindow: Loading... ", logger::LogLevel::INF);
}

void MainWindow::on_loadProgress(int progress)
{
    if (progress > 99)
        this->ui->webEngineView->stop();
    //this->m_logger.log("mainwindow: Loading... " + QString::number(progress) + "%", logger::LogLevel::INF);
}

void MainWindow::on_urlChanged(QUrl url)
{
    this->ui->eGbUrl->setText(url.toString());
}

//TODO: no [x] when working!
//TODO: disable edit, show wait icon animation when working!
