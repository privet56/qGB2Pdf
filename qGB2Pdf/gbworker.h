#ifndef GBWORKER_H
#define GBWORKER_H

#include <QFile>
#include <QObject>
#include <QTimer>
#include <QWebEngineView>
#include "util/cfg.h"
#include "util/logger.h"
#include "gbwriter.h"

class GbWorker : public QObject
{
    Q_OBJECT
public:
    explicit GbWorker(QObject *parent, QWebEngineView* pWebEngineView, logger* pLogger);

protected:
    logger* m_pLogger;

protected:
    QWebEngineView* m_pWebEngineView;
    QString m_sgb2pdf_js;

    GbWriter m_gbWriter;
    QTimer m_loadFinishedTimer;

    int m_iPage;

    void endScraping();

public:
    void startScrapingWithCurrentPage();

public slots:
    void on_loadFinished(bool ok);
    void clickNextPage();
    void on_loadStarted();
    void on_loadProgress(int progress);
    void on_JsInjected();

signals:
    void scrapFinished(QString sFN);
};

#endif // GBWORKER_H
