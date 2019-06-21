#ifndef DATAFILE_H
#define DATAFILE_H

#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QTextStream>

class DataFile : public QObject
{
    Q_OBJECT
public:
    explicit DataFile(QObject *parent = nullptr);
    ~DataFile();

    bool saveFile(const QString &FileName);
    bool saveFrame(QStringList &FileData);
    inline void closeFile()  { m_File->close(); }

signals:

public slots:

private:
    QFile *m_File;
    QTextStream m_Stream;
};

#endif // DATAFILE_H
