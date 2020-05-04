#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "def.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_pGbWorker(nullptr)
{
    ui->setupUi(this);
    QString sUrl(this->ui->eGbUrl->text());
    on_eGbUrl_textChanged(sUrl);
    if (sUrl.length() > 3)
        on_eGbUrl_returnPressed();
    connect(this->ui->webEngineView, SIGNAL(loadFinished(bool)), this, SLOT(on_loadFinished(bool)));
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
    m_pGbWorker = new GbWorker(this, this->ui->webEngineView);
    connect(this->m_pGbWorker, SIGNAL(scrapFinished(QString)), this, SLOT(scrapFinished(QString)));
    m_pGbWorker->startScrapingWithCurrentPage();
}

void MainWindow::on_eGbUrl_textChanged(const QString &gbUrl)
{
    if (m_pGbWorker) return;
    this->ui->bConvert->setEnabled(gbUrl.length() > 5 ? true : false);
}

void MainWindow::on_loadFinished(bool ok)
{
    if (!ok || this->m_pGbWorker) {
        this->ui->bConvert->setEnabled(false);
        return;
    }
    //TODO: move this logic into GbWorker!?
    this->ui->webEngineView->page()->toHtml([this](QString html)
    {
        bool gbPage = html.indexOf(__pageContaner) > 9 && html.indexOf(__cardNext) > 9;
        this->ui->bConvert->setEnabled(gbPage);
    });
}

void MainWindow::scrapFinished(QString sFN)
{
    qDebug() << sFN << "\n FINISHED";
    disconnect(this->m_pGbWorker, SIGNAL(scrapFinished(QString)), this, SLOT(on_loadFinished(bool)));
    this->m_pGbWorker->deleteLater();
    this->m_pGbWorker = nullptr;

    QString sUrl(this->ui->eGbUrl->text());
    this->on_eGbUrl_textChanged(sUrl);
}

//TODO: no [x] when working!
//TODO: disable edit, show wait icon when working!
