#ifndef TUMBLER_H
#define TUMBLER_H

#include <QFile>
#include <QObject>
#include <QTimer>
#include <QWebEngineView>
#include "util/cfg.h"
#include "util/logger.h"
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QEventLoop>
#include <QHttpPart>
#include <QUrl>
#include <QSslConfiguration>

class Tumbler : public QObject
{
    Q_OBJECT
public:
    explicit Tumbler(QObject *parent, QWebEngineView* pWebEngineView, logger* pLogger);

public:
    void dwnld(QString url);

protected:
    logger* m_pLogger;
    int m_iPage;
    int m_iStart;
    int m_iNumPerPage;
    QString m_sUserName;
    QSet<QString> m_targetUrls;

protected:
    QWebEngineView* m_pWebEngineView;

public slots:
    void on_loadFinished(bool ok);
    void on_loadStarted();
    void on_loadProgress(int progress);
    void dwnldNextPage();

#ifdef DOWNLOADWITHMANAGER
    QNetworkAccessManager manager;

public slots:
    void on_loadFinished();
    void slot_downloadReady();
    void sslErrors(QNetworkReply* reply, const QList<QSslError> & errors);
    void error(QNetworkReply::NetworkError code);
    void sslErrors(const QList<QSslError> & errors);
#endif
};

#endif // TUMBLER_H
