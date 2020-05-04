#ifndef ANILABEL_H
#define ANILABEL_H

#include <QLabel>
#include <QMovie>
#include <QIcon>
#include <QDebug>

class anilabel : public QLabel
{
    Q_OBJECT
public:
    explicit anilabel(QWidget *parent = nullptr);

    bool setani(QString sRes);
    void cleanup();
    QMovie* m_pMovie;

signals:

public slots:
    void setLabelIcon(int frame);
};

#endif // ANILABEL_H
