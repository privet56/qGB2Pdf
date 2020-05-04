#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gbworker.h"

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
    logger m_logger;
    cfg m_cfg;

protected:
    void closeEvent(QCloseEvent *evt);

private slots:
    void on_eGbUrl_returnPressed();

    void on_bConvert_clicked();

    void on_eGbUrl_textChanged(const QString &arg1);

    void on_loadFinished(bool ok);

    void scrapFinished(QString sFN);

    void loadStarted();

    void loadProgress(int progress);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
