#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTableWidgetItem>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>

#include <QByteArray>

#include "qwcomboboxdelegate.h"
#include "dataframe.h"
#include "datatype.h"
#include "datafile.h"

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
    void createItemARow(int row, QString name, QString type, bool check_item = false, QString data = "0");
    //储存数据帧数据类型
    void restoreType();
    void recognizeType(const QString *str);
    //串口设置
    void setSerialPort();
    //数据帧 数据组 更新
    void data_update(int type, int row);
    //表格 序号组 刷新
    void number_refresh();
    //接受帧计数Label清零
    void framecnt_zero();
    //数据帧Label长度更新
    void framelen_update();
    //保存数据帧配置
    void saveFrame(DataFile &datafile);
    //加载数据帧配置
    void loadFrame(int row, QString datastr);
    //检查Save文件夹
    QString check_SaveFloder();
    //保存程序关闭时数据帧配置
    void lastFrame();
    //获取表格刷新时间
    uint getRefreshTime(int index);
    //设置表格刷新频率
    void setRefreahTime(uint msec);


    //将接收字节转换成为各类型变量
    ushort ByteToUnShort(uchar *pByte) const;
    qint16 ByteToShort(uchar *pByte) const;
    uint   ByteToUnInt(uchar *pByte) const;
    qint32 ByteToInt(uchar *pByte) const;
    float  ByteToFloat(uchar *pByte) const;
    qreal  ByteToDouble(uchar *pByte) const;
    unsigned long ByteToUnLong(uchar *pByte) const;
    long   ByteToLong(uchar *pByte) const;

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
    void on_save_bt_clicked();
    void on_load_bt_clicked();

    void receive_frame(uchar *p_data);
    void table_update();
    void wait_read();
    void read_portdata();

    void on_refresh_box_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    DataFrame m_dataframe;
    DataFile  *m_datafile;
    //自定义代理
    QWComboBoxDelegate comboboxDelegate;
    //数据类型列表
    QList<int> m_typelist;
    //帧头/帧尾位置列表
    QList<int> m_headlist;
    //帧头/帧尾数据
    QByteArray m_headdata;
    //添加串口类
    QSerialPort *m_serial;
    //表格刷新定时器
    QTimer m_Refresh_Timer;

    uchar *m_itemdata;
    int m_framecnt;
    uint m_refresh_time;
    bool wait_flag;
};

#endif // MAINWINDOW_H
