#ifndef GBWORKER_H
#define GBWORKER_H

#include <QFile>
#include <QObject>
#include <QWebEngineView>

class GbWorker : public QObject
{
    Q_OBJECT
public:
    explicit GbWorker(QObject *parent, QWebEngineView* pWebEngineView);

protected:
    QWebEngineView* m_pWebEngineView;
    QString m_sgb2pdf_js;
    QFile m_fScrapedContent;
    QString m_sScrapedFN;
    QTextStream m_sScrapedContent;

    int m_iPage;

    void clickNextPage();
    void endScraping();

public:
    void startScrapingWithCurrentPage();

public slots:
    void on_loadFinished(bool ok);

signals:
    void scrapFinished(QString sFN);
};

#endif // GBWORKER_H
