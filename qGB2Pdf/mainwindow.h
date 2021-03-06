#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gbworker.h"
#include "tumbler.h"

#include "util/cfg.h"
#include "util/logger.h"
#include "util/logwnd.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    GbWorker* m_pGbWorker;
    Tumbler* m_pTumbler;
    logger m_logger;
    cfg m_cfg;

protected:
    void closeEvent(QCloseEvent *evt);
    void checkQrc();

private slots:
    void on_eGbUrl_returnPressed();

    void on_bConvert_clicked();

    void on_eGbUrl_textChanged(const QString &arg1);

    void on_loadFinished(bool ok);

    void scrapFinished(QString sFN);

    void on_loadStarted();

    void on_loadProgress(int progress);
    void on_urlChanged(QUrl url);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
