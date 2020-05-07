#include "anibutton.h"

anibutton::anibutton(QWidget *parent) : QPushButton(parent), m_pMovie(nullptr)
{

}

bool anibutton::setani(QString sRes)
{
    if (m_pMovie)
    {
        m_pMovie->stop();
        disconnect(m_pMovie,SIGNAL(frameChanged(int)),this,SLOT(setButtonIcon(int)));
        disconnect(m_pMovie,SIGNAL(finished()),m_pMovie,SLOT(start()));
        m_pMovie->deleteLater();
        m_pMovie = nullptr;
    }

    m_pMovie = new QMovie(sRes);
    connect(m_pMovie,SIGNAL(frameChanged(int)),this,SLOT(setButtonIcon(int)));
    if (m_pMovie->loopCount() != -1) //if movie doesn't loop forever, force it to
        connect(m_pMovie,SIGNAL(finished()),m_pMovie,SLOT(start()));
    m_pMovie->start();
    if(!m_pMovie->isValid())
    {
        //TODO: log correctly!
        //helpers::log("anibutton:setani NOT.ok("+sRes+"):"+QString::number(pMovie->isValid()), LOG_WRN, qApp, 0);
        qDebug() << QImageReader::supportedImageFormats();
        return false;
    }
    return true;
}

void anibutton::setButtonIcon(int frame)   // member function that catches the frameChanged signal of the QMovie
{
    if(!this->isChecked())return;
    Q_UNUSED(frame)
    QMovie* pMovie = (QMovie*)sender();
    this->setIcon(QIcon(pMovie->currentPixmap()));
}
