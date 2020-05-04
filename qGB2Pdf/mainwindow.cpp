#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
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
    QString sUrl(this->ui->eGbUrl->text());
    this->ui->webEngineView->load(QUrl(sUrl));
    this->ui->bConvert->setEnabled(false);
}

void MainWindow::on_bConvert_clicked()
{

}

void MainWindow::on_eGbUrl_textChanged(const QString &gbUrl)
{
    this->ui->bConvert->setEnabled(gbUrl.length() > 5 ? true : false);
}

void MainWindow::on_loadFinished(bool ok)
{
    this->ui->webEngineView->page()->toHtml([this](QString html)
    {
        bool gbPage = html.indexOf(__pageContaner) > 9 && html.indexOf(__cardNext) > 9;
        this->ui->bConvert->setEnabled(gbPage);
    });
}
