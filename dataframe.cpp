#include <QDebug>

#include "dataframe.h"
#include "cstring"

DataFrame::DataFrame(QList<int> *list, QList<int> *h_list, QByteArray *h_data):
    framelength(0),
    datacnt(0),
    framelist(list),
    headlist(h_list),
    headdata(h_data)
{
    QListIterator<int> frame(*list);
    while (frame.hasNext()) {
        switch (frame.next()) {
        case 0: framelength++; break;     //unsigned char
        case 1: framelength++; break;     //signed char
        case 2: framelength += 2; break;  //unsigned short
        case 3: framelength += 2; break;  //signed short
        case 4: framelength += 4; break;  //unsigned int
        case 5: framelength += 4; break;  //signed int
        case 6: framelength += 4; break;  //float
        case 7: framelength += 8; break;  //double
        case 8: framelength += 8; break;  //unsigned long int
        case 9: framelength += 8; break;  //signed long int
        case 10: framelength +=2; break;  //half word
        case 11: framelength +=4; break;  //full word
        }
    }
    qDebug() << "数据帧长度: " << framelength;

    //分离帧头帧尾
    SplitTail();
    headlen = headlist->size();
    taillen = taillist->size();
//    qDebug() << "帧头位置: " << *headlist;
//    qDebug() << "帧头数据: " << headdata->at(0) << " " << headdata->at(1);
//    if(!(taillist->empty())) {
//        qDebug() << "帧尾位置: " << *taillist;
//        qDebug() << "帧尾数据: " << taildata->at(0);
//    }
}

DataFrame::~DataFrame()
{
    delete taillist;
    delete taildata;
//    delete p_data;
}

void DataFrame::SplitTail()
{
    taillist = new QList<int>;
    taildata = new QByteArray;
    for (int i=0; i<headlist->size()-1; ++i) {
        //假如帧头列表不连续则视为帧尾
        if (headlist->at(i+1) > (headlist->at(i)+1)) {
            //移除headlist/headdata中帧尾列表和数据
            for (int j=headlist->size()-1; j>i; --j) {
                taillist->prepend(headlist->takeAt(j));
                taildata->append(headdata->at(j));
            }
            headdata->remove(i+1,taillist->size());
            break;
        }
    }
}

void DataFrame::ReadData(QByteArray *buf)
{
    uchar data[framelength];
    p_data = new uchar[framelength];
    for (int i=0; i<buf->size(); ++i) {
        data[datacnt] = static_cast<uchar>(buf->at(i));

        //判断帧头
        if(datacnt < headlen) {
            if (data[datacnt] == static_cast<uchar>(headdata->at(datacnt)))  {
                ++datacnt;
            }
            else {
                datacnt = 0;
            }
            continue;
        }

        //判断帧尾
        if(datacnt>(framelength-taillen-1)) {
            if (data[datacnt] != static_cast<uchar>(taildata->at(taillen-(framelength-datacnt)))) {
                datacnt = 0;
                continue;
            }
        }

        ++datacnt;

        //数据帧接收完整发出信号
        if(datacnt == framelength) {
            memcpy(p_data, data, framelength);
            datacnt = 0;
            send();
            break;
        }


    }
}

void DataFrame::send()
{
    emit frame_ok(p_data);
}



























