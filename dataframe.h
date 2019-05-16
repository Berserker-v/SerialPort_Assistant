#ifndef DATAFRAME_H
#define DATAFRAME_H
#include <QObject>
#include <QtGlobal>
#include <QByteArray>

#include "datatype.h"

class DataFrame : public QObject
{
    Q_OBJECT
private:

    void SplitTail();
    void send();

public:
    DataFrame(QList<int> *list, QList<int> *h_list, QByteArray *h_data);
    ~DataFrame();
    void ReadData(QByteArray * buf);

    inline int getFrameLen() { return framelength; }
    inline int getHeadLen()  { return headlist->size(); }
    inline int getTailLen()  { return taillen; }

signals:
    void frame_ok(uchar *p_data);

private:
    int framelength;
    int datacnt;
    int headlen;
    int taillen;
    uchar *p_data;
    QList<int> *framelist;
    QList<int> *headlist;
    QList<int> *taillist;
    QByteArray *headdata;
    QByteArray *taildata;
};

#endif // DATAFRAME_H
