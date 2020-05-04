#ifndef LOGWNDCONTROL_H
#define LOGWNDCONTROL_H

#include <QWidget>
#include "logger.h"

namespace Ui {
class LogWndControl;
}

class LogWndControl : public QWidget
{
    Q_OBJECT

public:
    explicit LogWndControl(QWidget *parent = 0);
    ~LogWndControl();
public:
    bool canLog2Wnd(QString s, logger::LogLevel level);

private slots:

private:
    Ui::LogWndControl *ui;
};

#endif // LOGWNDCONTROL_H
