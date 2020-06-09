#include "tumbler.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QNetworkProxyFactory>
#include <QNetworkReply>
#include <QProcess>
#include <QRegularExpressionMatchIterator>
#include <QTimer>
#include "def.h"
#include "util/str.h"

Tumbler::Tumbler(QObject *parent, QWebEngineView* pWebEngineView, logger* pLogger) : QObject(parent)
  , m_pLogger(pLogger)
  , m_iPage(0)
  , m_iStart(0)
  , m_iNumPerPage(99)
  , m_pWebEngineView(pWebEngineView)
{

}

void Tumbler::dwnld(QString userName)
{
    m_iPage = 0;
    m_iStart = 0;
    m_sUserName = userName;
    m_targetUrls.clear();

    connect(this->m_pWebEngineView, SIGNAL(loadFinished(bool)), this, SLOT(on_loadFinished(bool)));
    connect(this->m_pWebEngineView, SIGNAL(loadStarted()),      this, SLOT(on_loadStarted()));
    connect(this->m_pWebEngineView, SIGNAL(loadProgress(int)),  this, SLOT(on_loadProgress(int)));

    dwnldNextPage();
}
void Tumbler::dwnldNextPage()
{
    QString sUrl = "http://" + m_sUserName + ".tumblr.com/api/read?type=photo&num="+QString::number(m_iNumPerPage)+"&start=" + QString::number(this->m_iStart);
    this->m_pWebEngineView->load(QUrl(sUrl));
}

void Tumbler::on_loadFinished(bool ok)
{
    this->m_iPage++;
    this->m_pWebEngineView->stop();
    this->m_pWebEngineView->page()->toPlainText([this](QString sXml)
    {
        QRegularExpression reA("<img src=\"(.*?)\"");
        int iFoundPics = 0;

        QRegularExpressionMatchIterator i = reA.globalMatch(sXml);
        while (i.hasNext())
        {
            QRegularExpressionMatch match = i.next();
            if (match.hasMatch()) {
                QString pic(match.captured(1));
                this->m_targetUrls.insert(pic);
                iFoundPics++;
                qDebug() << "<" << pic << ">";
            }
        }
        if (iFoundPics > 3)
        {
            //TODO: finish implementation!!!

            m_iStart += m_iNumPerPage;
            qDebug() << "PIC URL page "+QString::number(m_iPage)+" finished! (startat:"+QString::number(m_iStart)+")(NumPerPage:"+QString::number(m_iNumPerPage)+") picsCount: "+QString::number(this->m_targetUrls.size());
            this->dwnldNextPage();
            QTimer::singleShot(99/*ms*/, this, SLOT(dwnldNextPage()));
        }
        else
        {
            qDebug() << "PIC URLs finished!";
        }

    });
}

void Tumbler::on_loadStarted()
{
    //m_pLogger->log("gbWorker: Load Started! ", logger::LogLevel::INF);
}

void Tumbler::on_loadProgress(int progress)
{

}

#ifdef DOWNLOADWITHMANAGER

void Tumbler::dwnld(QString userName)
{
    //TODO: type=video
    QString url = "http://" + userName + ".tumblr.com/api/read?type=photo&num=50&start=0";
    url = "https://pinayfarang.tumblr.com/api/read?type=photo&num=50&start=0";
    //url = "http://www.google.de/";

    QNetworkRequest* pNReq = new QNetworkRequest();
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    QUrl u(url, QUrl::StrictMode);
    pNReq->setUrl(u);
    {
       QSslConfiguration cfg = pNReq->sslConfiguration();
       cfg.setProtocol(QSsl::UnknownProtocol);
       pNReq->setSslConfiguration(cfg);
    }

    QNetworkReply *response = manager.get(QNetworkRequest(u));
    //QEventLoop event; //that would be the sync way...
    connect(response,SIGNAL(finished()),this,SLOT(on_loadFinished()));

    connect(response, SIGNAL(readyRead()), this, SLOT(slot_downloadReady()));
    connect(response, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
    connect(response, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(sslErrors(const QList<QSslError>&)));

    //event.exec();
    //QString xml = response->readAll();
}

void Tumbler::on_loadFinished()
{
    QNetworkReply* pReply = (QNetworkReply*)sender();
    // check for error
    if (pReply->error())
    {
        QString msg("Download of " + QString(pReply->url().toEncoded().constData()) + " failed: " + pReply->errorString());
        qDebug() << msg;
        qDebug() << "appdir: " << QApplication::applicationDirPath();
        qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();
        return;
    }

    QString xml(pReply->readAll());
    qDebug() << "xml:" << xml;

    pReply->deleteLater();
}
void Tumbler::slot_downloadReady()
{
    qDebug() << "slot_downloadReady";
}
void Tumbler::sslErrors(QNetworkReply* reply, const QList<QSslError>& errors)
{
    Q_UNUSED(reply)
    qDebug() << "sslErrors";
    for(int i=0; i < errors.length(); i++)
    {
        QSslError e = errors.at(i);
        qDebug() << e.errorString();
    }
}
void Tumbler::error(QNetworkReply::NetworkError code)
{
    //5     = QNetworkReply::OperationCanceledError
    //6     = QNetworkReply::SslHandshakeFailedError
    //204   = No Content
    //299   = QNetworkReply::UnknownContentError
    qDebug() << "error" << code;
}
void Tumbler::sslErrors(const QList<QSslError> & errors)
{
    qDebug() << "sslErrors2";
}
#endif
