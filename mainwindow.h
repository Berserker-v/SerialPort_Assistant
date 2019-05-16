#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTableWidgetItem>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QByteArray>

#include "qwcomboboxdelegate.h"
#include "dataframe.h"
#include "datatype.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    //自定义单元格类型
    enum CellType {ctCount=1000, ctName, ctType, ctData };
    //表格列号
    enum FieldColNum {colCount=0, colName, colType, colData };
    //QTablewidget创建行元素
    void createItemARow(int row, bool check_item = false);    
    //储存数据帧数据类型
    void restoreType();
    void recognizeType(QString *str);
    //串口设置
    void setSerialPort();
    //
    void data_update(int type, int row);

    //将接收字节转换成为各类型变量
    ushort ByteToUnShort(uchar *pByte);
    qint16 ByteToShort(uchar *pByte);
    uint   ByteToUnInt(uchar *pByte);
    qint32 ByteToInt(uchar *pByte);
    float  ByteToFloat(uchar *pByte);
    qreal  ByteToDouble(uchar *pByte);
    unsigned long ByteToUnLong(uchar *pByte);
    long   ByteToLong(uchar *pByte);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addRow_clicked();
    void on_delRow_clicked();
    void on_insertRow_clicked();
    void on_insertHead_clicked();
    void on_Complete_bt_clicked();
    void on_tableWidget_itemChanged(QTableWidgetItem *item);
    void on_delAll_clicked();
    void on_insertTail_clicked();
    void on_port_btn_clicked();

    void table_update(uchar *p_data);
    void read_portdata();

private:
    Ui::MainWindow *ui;
    DataFrame *m_dataframe;  //自定义代理
    QWComboBoxDelegate comboboxDelegate;
    //数据类型列表
    QList<int> *m_typelist;
    //帧头/帧尾位置列表
    QList<int> *m_headlist;
    //帧头/帧尾数据
    QByteArray *m_headdata;
    //添加串口类
    QSerialPort *m_serial;

    uchar *m_itemdata;
};

#endif // MAINWINDOW_H
