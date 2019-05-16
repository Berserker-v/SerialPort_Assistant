#ifndef DATATYPE_H
#define DATATYPE_H

#include <QtGlobal>

//数据类型
enum DataType {uchartype=0,  chartype,  ushorttype,    shorttype,
                  uinttype,   inttype,   floattype,   doubletype,
                 ulongtype,  longtype,    halftype,     fulltype };

//定义联合体，方便转换
union ByteAndShort {
    ushort ushort16;
    qint16 short16;
    uchar byte[2];
};

union ByteAndInt {
    uint uint32;
    qint32 int32;
    uchar byte[4];
};

union ByteAndFloat {
    float float32;
    uchar byte[4];
};

union ByteAndDouble {
    qreal double64;
    uchar byte[8];
};

union ByteAndLong {
    unsigned long ulong32;
    long long32;
    uchar byte[4];
};

#endif // DATATYPE_H
