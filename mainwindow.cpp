/**
* @projectName   %{FILENAME}
* @brief         摘要
* @author        DY
* @date          2019-05-18
*/
#include <QDate>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QComboBox>
#include <QDir>

#include <QtDebug>
#include <QFileDialog>

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
    //检查并加载程序最后退出时保存数据帧
//    QString savepath = check_SaveFloder();
//    QDir dir;
//    dir.setCurrent(savepath);

//    QFile lastFile("last.txt");

//    if (lastFile.exists())  {
//        if (lastFile.open(QIODevice::ReadOnly|QIODevice::Text))  {
//            QTextStream load_stream(&lastFile);
//            //清楚表格
//            on_delAll_clicked();
//            //添加表格项
//            int curRow = 0;
//            while(!load_stream.atEnd())  {
//                loadFrame(curRow, load_stream.readLine());
//                ++curRow;
//            }
//            lastFile.close();
//            /*----------------Printf Information-----------------*/
//            qDebug() << "Load last setting.";
//            /*---------------------------------------------------*/
//        }
//    }
//    else  {
//        /*----------------Printf Information-----------------*/
//        qDebug() << "First Start.";
//        /*---------------------------------------------------*/
//    }

    m_Refresh_Timer.stop();
}

MainWindow::~MainWindow()
{
    //程序退出时默认将最后数据帧保存
    QString savePath = check_SaveFloder();
    QDir dir;
    dir.setCurrent(savePath);
    lastFrame();

    delete ui;
}

QString MainWindow::check_SaveFloder()
{
    QString curPath = QDir::currentPath()+"/debug";
    QDir dir(curPath);
    QString savePath = curPath+ "/Save";
    if(!dir.exists(savePath))  {
        dir.mkdir("Save");
    }
    return savePath;
}

void MainWindow::lastFrame()
{
    DataFile File_save;
    QFile lastFile;
    if (lastFile.exists("last.txt"))  {
         File_save.saveFile("last.txt");
    }
    else  {
        lastFile.setFileName("last.txt");
        File_save.saveFile("last.txt");
    }
    //保存数据帧
    saveFrame(File_save);
    File_save.closeFile();
    /*----------------Printf Information-----------------*/
    qDebug() << "Save last setting";
    /*---------------------------------------------------*/
}

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
    number_refresh();
}

void MainWindow::on_insertHead_clicked()
{
    ui->tableWidget->insertRow(0);
    createItemARow(0, true);
    number_refresh();
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
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    ui->tableWidget->setItem(row, MainWindow::colCount, item);
    //数据名称
    str="<键入名称>";
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
    item = new QTableWidgetItem(str, MainWindow::ctData);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    ui->tableWidget->setItem(row, MainWindow::colData, item);
}

void MainWindow::number_refresh()
{
    int row_total = ui->tableWidget->rowCount();
    for (int i = 0; i < row_total; ++i) {
        QString str = QString::number(i+1);
        ui->tableWidget->item(i,colCount)->setText(str);
    }
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
        ui->insertTail->setEnabled(false);
        ui->delRow->setEnabled(false);
        ui->delAll->setEnabled(false);
        ui->load_bt->setEnabled(false);

        //使能 打开串口按钮
        ui->port_btn->setEnabled(true);

        //储存数据帧数据类型及帧头帧尾位置
        restoreType();

        m_dataframe.setFrame(m_typelist, m_headlist, m_headdata);

        //数据帧长度更新
        framelen_update();

        //接收数据帧完整信号
        connect(&m_dataframe,     &DataFrame::frame_ok,
                this,            &MainWindow::receive_frame);

        //表格数据类型栏不可编辑
        for (int i = 0; i < rowtotal; ++i) {
            item = ui->tableWidget->item(i,colType);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }

        /*----------------Printf Information-----------------*/
        qDebug() << "Comfirm Frame";
        /*---------------------------------------------------*/
    }
    else {
        ui->Complete_bt->setText(tr("确认数据帧"));
        /*complete_flag = false*/
        complete_flag = !complete_flag;

        //使能表格编辑按钮
        ui->addRow->setEnabled(true);
        ui->insertRow->setEnabled(true);
        ui->insertHead->setEnabled(true);
        ui->insertTail->setEnabled(true);
        ui->delRow->setEnabled(true);
        ui->delAll->setEnabled(true);
        ui->load_bt->setEnabled(true);

        ui->port_btn->setEnabled(false);

        //清楚数据类型列表
        m_typelist.clear();
        m_headlist.clear();
        m_headdata.clear();
        m_dataframe.clear();

        //使表格数据类型栏可以编辑
        for (int i = 0; i < rowtotal; ++i) {
            item = ui->tableWidget->item(i,colType);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        }

        /*----------------Printf Information-----------------*/
        qDebug() << "Edit Frame";
        /*---------------------------------------------------*/
    }
}

void MainWindow::restoreType()
{
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
           m_headlist.append(i);
           m_headdata.append(static_cast<char>(ui->tableWidget->item(i, colData)->text().toInt(&ok,16)));
       }
    }
}

void MainWindow::recognizeType(const QString *str)
{
    if (*str == "uchar") {
        m_typelist.append(DataType::uchartype);
    }
    else if (*str == "char") {
        m_typelist.append(DataType::chartype);
    }
    else if (*str == "ushort") {
        m_typelist.append(DataType::ushorttype);
    }
    else if (*str == "short") {
        m_typelist.append(DataType::shorttype);
    }
    else if (*str == "uint") {
        m_typelist.append(DataType::uinttype);
    }
    else if (*str == "int") {
        m_typelist.append(DataType::inttype);
    }
    else if (*str == "float") {
        m_typelist.append(DataType::floattype);
    }
    else if (*str == "double") {
        m_typelist.append(DataType::doubletype);
    }
    else if (*str == "ulong") {
        m_typelist.append(DataType::ulongtype);
    }
    else if (*str == "long") {
        m_typelist.append(DataType::longtype);
    }
    else if (*str == "halfword") {
        m_typelist.append(DataType::halftype);
    }
    else if (*str == "fullword") {
        m_typelist.append(DataType::fulltype);
    }
}

void MainWindow::receive_frame(uchar *p_data)
{
    m_itemdata = p_data;
    m_framecnt++;
    if (!m_refresh_time)  {
        table_update();
    }
}

//接收完整数据帧更新表格
void MainWindow::table_update()
{
    uchar *p_temp = m_itemdata;

    m_itemdata += m_dataframe.getHeadLen();
    for (int i = m_dataframe.getHeadLen();
         i < (ui->tableWidget->rowCount() - m_dataframe.getTailLen());
         ++i) {
        data_update(m_typelist.at(i), i);
    }
    ui->acceptnum_lb->setNum(m_framecnt);
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

void MainWindow::framelen_update()
{
    ui->lennum_lb->setNum(m_dataframe.getFrameLen());
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
        //连接信号槽
        QObject::connect(m_serial,  &QSerialPort::readyRead,
                         this,      &MainWindow::wait_read);
        wait_flag = true;
        //串口接收帧计数清零
        framecnt_zero();

        //设置表格刷新频率
        m_refresh_time = getRefreshTime(ui->refresh_box->currentIndex());
        setRefreahTime(m_refresh_time);
        m_Refresh_Timer.start();

        /*----------------Printf Information-----------------*/
        qDebug() << "Serial Port Open.";
        /*---------------------------------------------------*/
    }
    else {
        //关闭串口
        m_serial->clear();
        m_serial->close();
        m_serial->deleteLater();

        m_Refresh_Timer.stop();

        /*----------------Delete Information-----------------*/
        qDebug() << "Free Memory...m_itemdata...";
        /*---------------------------------------------------*/
        delete m_itemdata;  m_itemdata=nullptr;
        /*---------------------------------------------------*/
        qDebug() << "Free Memory Successfully!";
        /*---------------------------------------------------*/

        //开启菜单
        ui->port_btn->setText(tr("打开串口"));
        ui->port_box->setEnabled(true);
        ui->baud_box->setEnabled(true);
        ui->data_box->setEnabled(true);
        ui->parity_box->setEnabled(true);
        ui->stop_box->setEnabled(true);

        /*----------------Printf Information-----------------*/
        qDebug() << "Serial Port Close.";
        /*---------------------------------------------------*/
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
    m_serial->setReadBufferSize(5*(m_dataframe.getFrameLen()));

    /*----------------Printf Information-----------------*/
    qDebug() << "Serial Port Buffersize:" << m_serial->readBufferSize();
    /*---------------------------------------------------*/
}

void MainWindow::setRefreahTime(uint msec)
{
    if(msec)  {
        m_Refresh_Timer.setInterval(static_cast<int>(msec));
        connect(&m_Refresh_Timer,     SIGNAL(timeout()),
                this,                 SLOT(table_update()));
    }
    else  {
        disconnect(&m_Refresh_Timer,  SIGNAL(timeout()),
                   this,              SLOT(table_update()));
        m_Refresh_Timer.stop();
    }
}

uint MainWindow::getRefreshTime(int index)
{
    switch (index) {
    case 0: return 0;
    case 1: return 1000;
    case 2: return 500;
    case 3: return 200;
    case 4: return 100;
    default:
        return 0;
    }
}

void MainWindow::wait_read()
{
    if (wait_flag)  {
        wait_flag = false;
        QTimer::singleShot(5, this, &MainWindow::read_portdata);
    }
//    read_portdata();
}

void MainWindow::read_portdata()
{
    QByteArray buf;
    buf = m_serial->readAll();
    wait_flag = true;
    m_dataframe.ReadData(&buf);
}

void MainWindow::framecnt_zero()
{
    m_framecnt = 0;
    ui->acceptnum_lb->setNum(m_framecnt);
}

void MainWindow::on_save_bt_clicked()
{
    DataFile File_save;
    //检查Save文件夹
    QString curPath = check_SaveFloder();

    //新建数据帧文件
    QString dlgTitle = tr("另存为");
    QString filter = "文本文件(*.txt);;所有文件(*.)";
    QString save_Filename = QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);

    if (save_Filename.isEmpty())
        return ;

    File_save.saveFile(save_Filename);
    //保存数据帧
    saveFrame(File_save);

    File_save.closeFile();

    /*----------------Printf Information-----------------*/
    qDebug() << "Save Successfully!";
    /*---------------------------------------------------*/

}

void MainWindow::saveFrame(DataFile &datafile)
{
    QStringList strlist;
    QTableWidgetItem *item;
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        item = ui->tableWidget->item(i,colName);
        strlist << item->text()
                << ui->tableWidget->item(i,colType)->text();
        //判断是否为帧头
        if(item->checkState() == Qt::Checked) {
            strlist << ui->tableWidget->item(i,colData)->text() <<"Yes";
        }
        //保存至文件
        datafile.saveFrame(strlist);
        strlist.clear();
    }
}

void MainWindow::on_load_bt_clicked()
{
    QString curPath = QDir::currentPath();
    curPath += "/debug";
    QDir dir(curPath);
    QString savePath = curPath+ "/save";
    if(dir.exists(savePath))  {
        curPath = savePath;
    }
    //Dialog配置
    QString dlgTitle = tr("加载配置文件");
    QString filter = "文本文件(*.txt);;所有文件(*.)";
    QString load_Filename = QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);

    if (load_Filename.isEmpty())
        return ;

    QFile file_Load(load_Filename);
    //判断文件是否打开
    if(!file_Load.open(QIODevice::ReadOnly|QIODevice::Text))  {
        /*----------------Printf Information-----------------*/
        qWarning() << "Load File Failed!";
        /*---------------------------------------------------*/
        return;
    }    
    QTextStream load_stream(&file_Load);
//    load_stream.setAutoDetectUnicode(true);
    //清楚表格
    on_delAll_clicked();
    //添加表格项
    int curRow = 0;
    while(!load_stream.atEnd())  {
        loadFrame(curRow, load_stream.readLine());
        ++curRow;
    }
    file_Load.close();
    /*----------------Printf Information-----------------*/
    qDebug() << "Load Successfully!";
    /*---------------------------------------------------*/
}

void MainWindow::loadFrame(int row, QString datastr)
{
    QStringList strlist = datastr.split(",");
    ui->tableWidget->insertRow(row);
    if (strlist.size() == 2)  {
        createItemARow(row,strlist.at(0),strlist.at(1));
    }
    else if (strlist.size() == 4)  {
        createItemARow(row,strlist.at(0),strlist.at(1),true,strlist.at(2));
    }
}

void MainWindow::createItemARow(int row, QString name, QString type, bool check_item, QString data)
{
    QTableWidgetItem *item;
    QString str;
    //为 新加行 设置item
    //序号
    str.setNum(row+1);
    item = new QTableWidgetItem(str, MainWindow::ctCount);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    ui->tableWidget->setItem(row, MainWindow::colCount, item);
    //数据名称
    item = new QTableWidgetItem(name, MainWindow::ctName);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    if(check_item)
    {
        //判断是否插入帧头
        item->setCheckState(Qt::Checked);
    }
    ui->tableWidget->setItem(row, MainWindow::colName, item);
    //数据类型
    item = new QTableWidgetItem(type, MainWindow::ctType);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->setItem(row, MainWindow::colType, item);
    //数值
    if(check_item)  {
        item = new QTableWidgetItem(data, MainWindow::ctData);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item->setFlags(item->flags()|Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, MainWindow::colData, item);
    }
    else {
        item = new QTableWidgetItem("0", MainWindow::ctData);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item->setFlags(item->flags()|Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, MainWindow::colData, item);
    }
}

void MainWindow::on_refresh_box_currentIndexChanged(int index)
{
    m_Refresh_Timer.stop();
    m_refresh_time = getRefreshTime(index);
    setRefreahTime(m_refresh_time);
    if (ui->port_btn->text() == tr("关闭串口"))
        m_Refresh_Timer.start();
}

//字节转各数据类型
ushort MainWindow::ByteToUnShort(uchar *pByte) const
{
    ByteAndShort data;
    data.byte[0] = *pByte;
    pByte++;
    data.byte[1] = *pByte;
    return data.ushort16;
}

qint16 MainWindow::ByteToShort(uchar *pByte) const
{
    ByteAndShort data;
    data.byte[0] = *pByte;
    pByte++;
    data.byte[1] = *pByte;
    return data.short16;
}

uint MainWindow::ByteToUnInt(uchar *pByte) const
{
    ByteAndInt data;
    for (int i=0; i<4; ++i) {
        data.byte[i] = *(pByte);
        pByte++;
    }
    return data.uint32;
}

qint32 MainWindow::ByteToInt(uchar *pByte) const
{
    ByteAndInt data;
    for (int i=0; i<4; ++i) {
        data.byte[i] = *(pByte);
        pByte++;
    }
    return data.int32;
}

float MainWindow::ByteToFloat(uchar *pByte) const
{
    ByteAndFloat data;
    for (int i=0; i<4; ++i) {
        data.byte[i] = *(pByte);
        pByte++;
    }
    return data.float32;
}

qreal MainWindow::ByteToDouble(uchar *pByte) const
{
    ByteAndDouble data;
    for (int i=0; i<8; ++i) {
        data.byte[i] = *(pByte);
        pByte++;
    }
    return data.double64;
}

unsigned long MainWindow::ByteToUnLong(uchar *pByte) const
{
    ByteAndLong data;
    for (int i=0; i<4; ++i) {
        data.byte[i] = *(pByte);
        pByte++;
    }
    return data.ulong32;
}

long MainWindow::ByteToLong(uchar *pByte)  const
{
    ByteAndLong data;
    for (int i=0; i<4; ++i) {
        data.byte[i] = *(pByte);
        pByte++;
    }
    return data.long32;
}
















