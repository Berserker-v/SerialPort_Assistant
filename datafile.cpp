#include "datafile.h"

#include <QDir>
#include <QDebug>

DataFile::DataFile(QObject *parent) : QObject(parent),
    m_File(nullptr)
{

}

DataFile::~DataFile()
{
    if (m_File != nullptr)  {
        /*----------------Delete Information-----------------*/
        qDebug() << "Free Memory...file";
        /*---------------------------------------------------*/
        delete m_File;
        /*---------------------------------------------------*/
        qDebug() << "Free Successfully!";
        /*---------------------------------------------------*/
    }
}

bool DataFile::saveFrame(const QString &FileName)
{
    m_File = new QFile(FileName);
    if(!m_File->open(QIODevice::WriteOnly|QIODevice::Text))
        return false;
    //创建文件文本流
    m_Stream.setDevice(m_File);

    return true;
}

bool DataFile::saveFrame(QStringList &FileData)
{
    QString str = FileData.join(",");
    m_Stream << str << endl;

    return true;
}









