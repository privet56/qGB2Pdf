#include "anilabel.h"
#include "util/f.h"

anilabel::anilabel(QWidget *parent) : QLabel(parent), m_pMovie(nullptr)
{

}

bool anilabel::setani(QString sRes)
{
    cleanup();

    sRes = f::getResFn(sRes);

    m_pMovie = new QMovie(sRes);
    connect(m_pMovie,SIGNAL(frameChanged(int)),this,SLOT(setLabelIcon(int)));
    if (m_pMovie->loopCount() != -1) //if movie doesn't loop forever, force it to
        connect(m_pMovie,SIGNAL(finished()),m_pMovie,SLOT(start()));
    m_pMovie->start();
    if(!m_pMovie->isValid())
    {
        //TODO: log correctly!
        //helpers::log("anibutton:setani NOT.ok("+sRes+"):"+QString::number(pMovie->isValid()), LOG_WRN, qApp, 0);
        qDebug()<<QImageReader::supportedImageFormats();
        return false;
    }
    return true;
}

void anilabel::setLabelIcon(int frame)   // member function that catches the frameChanged signal of the QMovie
{
    //if(!this->isChecked())return;
    Q_UNUSED(frame)
    QMovie* pMovie = (QMovie*)sender();
    this->setPixmap(pMovie->currentPixmap());
}

void anilabel::cleanup()
{
    if(this->m_pMovie)
    {
        this->m_pMovie->disconnect();
        this->m_pMovie->deleteLater();
        delete this->m_pMovie;
        this->m_pMovie = nullptr;
    }
}
