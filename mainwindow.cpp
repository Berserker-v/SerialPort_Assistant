#include <QDate>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QComboBox>



#include <QTimer>
#include <QRandomGenerator>
#include <QtDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_itemdata(nullptr)
{
    ui->setupUi(this);

    //将 数据类型 设置为ComboBox代理组件
    ui->tableWidget->setItemDelegateForColumn(colType, &comboboxDelegate);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 槽函数
void MainWindow::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    static bool item_flag = true ;

    int col = item->column();
    if( col == 2 && item_flag)
    {       
        //将单元格内数据设置对齐之后，会再次触发信号，所以需要将其屏蔽
        /*item_flag = false*/
        item_flag = !item_flag;
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        /*item_flag = true*/
        item_flag = !item_flag;
    }
//    if( col == 3 && item_flag)
//    {
//        //将单元格内数据设置对齐之后，会再次触发信号，所以需要将其屏蔽
//        /*item_flag = false*/
//        item_flag = !item_flag;
//        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
//        /*item_flag = true*/
//        item_flag = !item_flag;
//    }
}

void MainWindow::on_addRow_clicked()
{
    int curRow = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(curRow);
    createItemARow(curRow);
}

void MainWindow::on_insertRow_clicked()
{
    int curRow = ui->tableWidget->currentRow();
    ui->tableWidget->insertRow(curRow);
    createItemARow(curRow);
}

void MainWindow::on_insertHead_clicked()
{
    ui->tableWidget->insertRow(0);
    createItemARow(0, true);
}

void MainWindow::on_insertTail_clicked()
{
    int curRow = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(curRow);
    createItemARow(curRow, true);
}

void MainWindow::createItemARow(int row, bool check_item )
{
    QTableWidgetItem *item;
    QString str;
    //为 新加行 设置item
    //序号
    str.setNum(row+1);
    item = new QTableWidgetItem(str, MainWindow::ctCount);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->setItem(row, MainWindow::colCount, item);
    //数据名称
    str.prepend("数据");
    item = new QTableWidgetItem(str, MainWindow::ctName);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    if(check_item)
    {
        //判断是否插入帧头
        item->setCheckState(Qt::Checked);
    }
    ui->tableWidget->setItem(row, MainWindow::colName, item);
    //数据类型
    item = new QTableWidgetItem("uchar", MainWindow::ctType);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->setItem(row, MainWindow::colType, item);
    //数值
    str.setNum(0);
//    item = new QTableWidgetItem(str, MainWindow::ctData);
    item = new QTableWidgetItem(str);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    ui->tableWidget->setItem(row, MainWindow::colData, item);
}

void MainWindow::on_delRow_clicked()
{
    int curRow = ui->tableWidget->currentRow();
    ui->tableWidget->removeRow(curRow);
}

void MainWindow::on_Complete_bt_clicked()
{
    static bool complete_flag = true;
    QTableWidgetItem *item;

    int rowtotal = ui->tableWidget->rowCount();

    if(complete_flag) {
        ui->Complete_bt->setText(tr("编辑数据帧"));
        /*complete_flag = true*/
        complete_flag = !complete_flag;

        //关闭表格编辑按钮
        ui->addRow->setEnabled(false);
        ui->insertRow->setEnabled(false);
        ui->insertHead->setEnabled(false);
        ui->delRow->setEnabled(false);

        //使能 打开串口按钮
        ui->port_btn->setEnabled(true);

        //储存数据帧数据类型及帧头帧尾位置
        restoreType();

        qDebug() << "数据帧帧头帧尾位置: " << *m_headlist ;

        m_dataframe = new DataFrame(m_typelist, m_headlist, m_headdata);

        //接收数据帧完整信号
        connect(m_dataframe,     &DataFrame::frame_ok,
                this,            &MainWindow::table_update);

        //表格数据类型栏不可编辑
        for (int i = 0; i < rowtotal; ++i) {
            item = ui->tableWidget->item(i,colType);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
    }
    else {
        ui->Complete_bt->setText(tr("确认数据帧"));
        /*complete_flag = false*/
        complete_flag = !complete_flag;

        //使能表格编辑按钮
        ui->addRow->setEnabled(true);
        ui->insertRow->setEnabled(true);
        ui->insertHead->setEnabled(true);
        ui->delRow->setEnabled(true);

        //清楚数据类型列表
        delete m_typelist;
        delete m_headlist;
        delete m_dataframe;

        //使表格数据类型栏可以编辑
        for (int i = 0; i < rowtotal; ++i) {
            item = ui->tableWidget->item(i,colType);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        }
    }
}

void MainWindow::restoreType()
{
    m_typelist = new QList<int>;
    m_headlist = new QList<int>;
    m_headdata = new QByteArray;
    QString typestr;
    QTableWidgetItem *item;
    bool ok = true;
    int rowTotal = ui->tableWidget->rowCount();

    for (int i = 0; i < rowTotal; ++i) {
        //判别数据类型并储存至列表内
       item = ui->tableWidget->item(i,colType);
       typestr = item->text();
       recognizeType(&typestr);

       //判别是否为数据帧并将其位置储存至列表内
       item = ui->tableWidget->item(i,colName);
       if (item->checkState() == Qt::Checked) {
           m_headlist->append(i);
           m_headdata->append(static_cast<char>(ui->tableWidget->item(i, colData)->text().toInt(&ok,16)));
       }
    }
/*----------------------测试--------------------------*/
    qDebug() << m_headdata->toHex();
/*---------------------------------------------------*/
}

void MainWindow::recognizeType(QString *str)
{
    if (*str == "uchar") {
        m_typelist->append(DataType::uchartype);
    }
    else if (*str == "char") {
        m_typelist->append(DataType::chartype);
    }
    else if (*str == "ushort") {
        m_typelist->append(DataType::ushorttype);
    }
    else if (*str == "short") {
        m_typelist->append(DataType::shorttype);
    }
    else if (*str == "uint") {
        m_typelist->append(DataType::uinttype);
    }
    else if (*str == "int") {
        m_typelist->append(DataType::inttype);
    }
    else if (*str == "float") {
        m_typelist->append(DataType::floattype);
    }
    else if (*str == "double") {
        m_typelist->append(DataType::doubletype);
    }
    else if (*str == "ulong") {
        m_typelist->append(DataType::ulongtype);
    }
    else if (*str == "long") {
        m_typelist->append(DataType::longtype);
    }
    else if (*str == "halfword") {
        m_typelist->append(DataType::halftype);
    }
    else if (*str == "fullword") {
        m_typelist->append(DataType::fulltype);
    }
}

//接收完整数据帧更新表格
void MainWindow::table_update(uchar *p_data)
{
    uchar *p_temp = p_data;

    m_itemdata = p_data;
    m_itemdata += m_dataframe->getHeadLen();
    for (int i = m_dataframe->getHeadLen();
         i < (ui->tableWidget->rowCount() - m_dataframe->getTailLen());
         ++i) {
        data_update(m_typelist->at(i), i);
    }

    m_itemdata = p_temp;
}

void MainWindow::data_update(int type, int row)
{
    QString str;
    switch (type) {
    case DataType::uchartype:
        str = QString::number(*m_itemdata, 16).toUpper();
        ++m_itemdata;
        ui->tableWidget->item(row,colData)->setText(str);
        break;
    case DataType::chartype:
        str = QString::number(*m_itemdata, 16).toUpper();
        ++m_itemdata;
        ui->tableWidget->item(row,colData)->setText(str);
        break;
    case DataType::ushorttype:
        str = QString::number(ByteToUnShort(m_itemdata));
        m_itemdata += 2;
        ui->tableWidget->item(row,colData)->setText(str);
        break;
    case DataType::shorttype:
        str = QString::number(ByteToShort(m_itemdata));
        m_itemdata += 2;
        ui->tableWidget->item(row,colData)->setText(str);
        break;
    case DataType::uinttype:
        str = QString::number(ByteToUnInt(m_itemdata));
        m_itemdata += 4;
        ui->tableWidget->item(row,colData)->setText(str);
        break;
    case DataType::inttype:
        str = QString::number(ByteToInt(m_itemdata));
        m_itemdata += 4;
        ui->tableWidget->item(row,colData)->setText(str);
        break;
    case DataType::floattype:
        str = QString::number(ByteToFloat(m_itemdata));
        m_itemdata += 4;
        ui->tableWidget->item(row,colData)->setText(str);
        break;
    case DataType::doubletype:
        str = QString::number(ByteToShort(m_itemdata));
        m_itemdata += 8;
        ui->tableWidget->item(row,colData)->setText(str);
        break;
    case DataType::ulongtype:
        str = QString::number(ByteToUnLong(m_itemdata));
        m_itemdata += 4;
        ui->tableWidget->item(row,colData)->setText(str);
        break;
    case DataType::longtype:
        str = QString::number(ByteToLong(m_itemdata));
        m_itemdata += 4;
        ui->tableWidget->item(row,colData)->setText(str);
        break;
    }
}

void MainWindow::on_delAll_clicked()
{
    int rowtotal = ui->tableWidget->rowCount();
    for (int i = rowtotal; i > 0; --i) {
       ui->tableWidget->removeRow(i-1);
    }
}

void MainWindow::on_port_btn_clicked()
{
    if( ui->port_btn->text() == tr("打开串口")) {
        //关闭串口菜单
        ui->port_btn->setText(tr("关闭串口"));
        ui->port_box->setEnabled(false);
        ui->baud_box->setEnabled(false);
        ui->data_box->setEnabled(false);
        ui->parity_box->setEnabled(false);
        ui->stop_box->setEnabled(false);

        //设置串口
        setSerialPort();
        qDebug() << m_serial->readBufferSize();
        //连接信号槽
        QObject::connect(m_serial,  &QSerialPort::readyRead,
                         this,      &MainWindow::read_portdata);
    }
    else {
        //关闭串口
        m_serial->clear();
        m_serial->close();
        m_serial->deleteLater();
        if( m_itemdata != nullptr)
        {
            delete m_itemdata;
        }

        //开启菜单
        ui->port_btn->setText(tr("打开串口"));
        ui->port_box->setEnabled(true);
        ui->baud_box->setEnabled(true);
        ui->data_box->setEnabled(true);
        ui->parity_box->setEnabled(true);
        ui->stop_box->setEnabled(true);
    }
}

void MainWindow::setSerialPort()
{
    m_serial = new QSerialPort;

    //串口号 读写 波特率
    m_serial->setPortName(ui->port_box->currentText());
    m_serial->open(QIODevice::ReadWrite);
    m_serial->setBaudRate(ui->baud_box->currentText().toInt());

    //数据位
    switch (ui->data_box->currentIndex()) {
    case 0:
        m_serial->setDataBits(QSerialPort::Data6);
        break;
    case 1:
        m_serial->setDataBits(QSerialPort::Data7);
        break;
    case 2:
        m_serial->setDataBits(QSerialPort::Data8);
        break;
    }

    //校验位
    switch (ui->parity_box->currentIndex()) {
    case 0:
        m_serial->setParity(QSerialPort::NoParity);
        break;
    case 1:
        m_serial->setParity(QSerialPort::OddParity);
        break;
    case 2:
        m_serial->setParity(QSerialPort::EvenParity);
        break;
    }

    //停止位
    switch (ui->stop_box->currentIndex()) {
    case 0:
        m_serial->setStopBits(QSerialPort::OneStop);
        break;
    case 1:
        m_serial->setStopBits(QSerialPort::OneAndHalfStop);
        break;
    case 2:
        m_serial->setStopBits(QSerialPort::TwoStop);
        break;
    }

    //关闭硬件控制流
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

    //设置串口接收缓冲区数据长度
    m_serial->setReadBufferSize(2*(m_dataframe->getFrameLen()));
}

void MainWindow::read_portdata()
{
    QByteArray buf;
    buf = m_serial->readAll();

/*----------------------测试--------------------------*/
    qDebug() << buf.toHex();
/*---------------------------------------------------*/
    m_dataframe->ReadData(&buf);
}

//字节转各数据类型
ushort MainWindow::ByteToUnShort(uchar *pByte)
{
    ByteAndShort data;
    data.byte[0] = *pByte;
    pByte++;
    data.byte[1] = *pByte;
    return data.ushort16;
}

qint16 MainWindow::ByteToShort(uchar *pByte)
{
    ByteAndShort data;
    data.byte[0] = *pByte;
    pByte++;
    data.byte[1] = *pByte;
    return data.short16;
}

uint MainWindow::ByteToUnInt(uchar *pByte)
{
    ByteAndInt data;
    for (int i=0; i<4; ++i) {
        data.byte[i] = *(pByte);
        pByte++;
    }
    return data.uint32;
}

qint32 MainWindow::ByteToInt(uchar *pByte)
{
    ByteAndInt data;
    for (int i=0; i<4; ++i) {
        data.byte[i] = *(pByte);
        pByte++;
    }
    return data.int32;
}

float MainWindow::ByteToFloat(uchar *pByte)
{
    ByteAndFloat data;
    for (int i=0; i<4; ++i) {
        data.byte[i] = *(pByte);
        pByte++;
    }
    return data.float32;
}

qreal MainWindow::ByteToDouble(uchar *pByte)
{
    ByteAndDouble data;
    for (int i=0; i<8; ++i) {
        data.byte[i] = *(pByte);
        pByte++;
    }
    return data.double64;
}

unsigned long MainWindow::ByteToUnLong(uchar *pByte)
{
    ByteAndLong data;
    for (int i=0; i<4; ++i) {
        data.byte[i] = *(pByte);
        pByte++;
    }
    return data.ulong32;
}

long MainWindow::ByteToLong(uchar *pByte)
{
    ByteAndLong data;
    for (int i=0; i<4; ++i) {
        data.byte[i] = *(pByte);
        pByte++;
    }
    return data.long32;
}














