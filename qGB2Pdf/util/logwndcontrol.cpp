#include "logwndcontrol.h"
#include "ui_logwndcontrol.h"

LogWndControl::LogWndControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogWndControl)
{
    ui->setupUi(this);
}

LogWndControl::~LogWndControl()
{
    delete ui;
}

bool LogWndControl::canLog2Wnd(QString s, logger::LogLevel level)
{
    switch (level)
    {
        case logger::LogLevel::ERR:
        {
            if (s.startsWith(logger::PREFIX_STDERR))
            {
                return this->ui->logwndcontrol_showSTDERR->isChecked();
            }
            return this->ui->logwndcontrol_showERR->isChecked();
        }
        case logger::LogLevel::WRN:
        {
            return this->ui->logwndcontrol_showWRN->isChecked();
        }
        case logger::LogLevel::INF:
        {
            if (s.startsWith(logger::PREFIX_STDOUT))
            {
                return this->ui->logwndcontrol_showSTDOUT->isChecked();
            }
            return this->ui->logwndcontrol_showINF->isChecked();
        }
        default:
        {
            return true;
        }
    }

    return true;
}
