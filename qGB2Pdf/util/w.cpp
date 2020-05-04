#include "w.h"
#include "str.h"

#include <QApplication>
#include <QMainWindow>
#include <QTimer>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QFileDialog>

#if defined(WIN32)
// you will need win32: LIBS += -luser32 in .pro
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#endif

w::w(QWidget *parent) : QWidget(parent)
{

}

QString w::getOpenFileName(QWidget* parent, QString sTitle, QString sPattern)
{
    static QString sDir(qApp->applicationDirPath());
    QString sAbsFN = QFileDialog::getOpenFileName(parent, sTitle, sDir, sPattern);

    if(!str::isempty(sAbsFN, true))
    {
        QFileInfo finfo(sAbsFN);
        sDir = finfo.path();
    }

    return sAbsFN;
}

#if defined(WIN32)
//#pragma commment(lib, "user32.lib")   =   pro:win32: LIBS += -luser32
BOOL CALLBACK EnumThreadWndProcAndClose(HWND hwnd, LPARAM lParam)
{
    Q_UNUSED(lParam)

#ifdef _DEBUG
    char WindowTitle[1024];
    ::GetWindowText(hwnd, (LPWSTR)WindowTitle, 1024);
    qDebug() << WindowTitle;
#endif

    SendMessage(hwnd, WM_CLOSE, 0, 0);
    return TRUE;
}
#endif

void w::killSubProcesses(qint64 pid, logger* pLogger, int iRecDepth/*=0*/)
{
#if defined(WIN32)
    // Take a snapshot of all processes in the system.
    //hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    // Take a snapshot of all modules in the specified process.
    //hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );
    // Take a snapshot of all running threads
    //hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if(hProcessSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32	processEntry32 ;
        processEntry32.dwSize = sizeof(PROCESSENTRY32);
        if(Process32First(hProcessSnap, &processEntry32))
        {
            do
            {
                if (pid == processEntry32.th32ParentProcessID)
                {
                    //qDebug() << QString::fromWCharArray(processEntry32.szExeFile);

                    //although, children "conhost.exe" & "java.exe" do not have hwnds...
                    ::EnumThreadWindows(processEntry32.th32ProcessID, EnumThreadWndProcAndClose, NULL);

                    w::killSubProcesses(processEntry32.th32ProcessID, pLogger, iRecDepth+1);  //recurse

                    HANDLE hChildProc = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry32.th32ProcessID);

                    if (hChildProc)
                    {
                       ::TerminateProcess(hChildProc, 1);
                       ::CloseHandle(hChildProc);
                    }
                    else
                    {
                       if(pLogger)pLogger->err("process couldn't be closed:"+QString::fromWCharArray(processEntry32.szExeFile));
                    }
                }
            }
            while(Process32Next(hProcessSnap, &processEntry32)) ;
        }
        CloseHandle(hProcessSnap);
    }
    else
    {
        if(pLogger)pLogger->err("could not list child processes to be closed");
    }

    if(iRecDepth == 0)
    {
        ::EnumThreadWindows(pid, EnumThreadWndProcAndClose, NULL);
    }

#else
    if(pLogger)pLogger->err("function killSubProcesses is not supported on non-windows!");
#endif
}
