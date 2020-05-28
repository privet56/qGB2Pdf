#ifndef DEF_H
#define DEF_H

#define __pageContaner QString("pageContainer")
#define __cardNext QString("cardNext")
#define __cssFN QString("res/css.css")

#include <QObject>

class def : public QObject
{
    Q_OBJECT
public:
    explicit def(QObject *parent = nullptr);

signals:

};

#endif // DEF_H
