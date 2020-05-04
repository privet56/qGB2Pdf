#include "logwnd.h"
#include <QScrollBar>
#include <QCheckBox>
#include <QDebug>
#include "logwndcontrol.h"

LogWnd::LogWnd(QWidget *parent) : QTextEdit(parent), m_pStatusBar(nullptr)
{
    m_pLogWndControl = new LogWndControl(this);
}

void LogWnd::resizeEvent(QResizeEvent *event)
{
    QTextEdit::resizeEvent(event);

    QSize sizeLogWnd(this->size());
    QSize sizeLogWndCtrl(this->m_pLogWndControl->size());
    int left = sizeLogWnd.width() - sizeLogWndCtrl.width();
    {
        //int w = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
        //QWidgetList scrollBarRight = this->scrollBarWidgets(Qt::AlignRight);
        if(this->verticalScrollBar()->isVisible())
            left -= this->verticalScrollBar()->width();
    }
    this->m_pLogWndControl->move(left, 0);
}

void LogWnd::SetStatusBar(QStatusBar* pStatusBar)
{
     this->m_pStatusBar = pStatusBar;
}

void LogWnd::AddMsg(QString s, logger::LogLevel level)
{
    if(!this->m_pLogWndControl->canLog2Wnd(s, level))
    {
        return;
    }
    bool bIsSameLine = (s.length() < 2);
    static bool __bLastLineWasIsSameLine = false;   //TODO: should be member

    QString s2log = bIsSameLine ? s : "<font color='"+logger::level2color(level)+"'>"+s+"</font>";
    if(bIsSameLine && __bLastLineWasIsSameLine)
    {
        this->insertHtml(s2log);
    }
    else
    {
        this->append(s2log);
    }

    QScrollBar *sb = this->verticalScrollBar();
    sb->setValue(sb->maximum());

    __bLastLineWasIsSameLine = bIsSameLine;

    if(!bIsSameLine && this->m_pStatusBar)
    {
        this->m_pStatusBar->showMessage(s);
    }

    //TODO: check if too much in the teLog win!
}
