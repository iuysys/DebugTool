#include "serialport.h"
#include "ui_serialport.h"

#include <QMessageBox>

SerialPort::SerialPort(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialPort)
{
    mSerialPort = new QSerialPort();
    connect(mSerialPort,SIGNAL(readyRead()),this,SLOT(EmitReadyRead()));

    mPortConf.PortStatus = false;

    ui->setupUi(this);
    ui->comboBox_SerialPort->view()->setFixedWidth(400);        //设置combobox本身宽度不变,下拉项目宽度变宽,ui属性里还需要设置最小宽度
    UpdataPortName();
}

SerialPort::~SerialPort()
{
    delete ui;
}

QByteArray SerialPort::readAll()
{
    return mSerialPort->readAll();
}

qint64 SerialPort::write(const char *data, qint64 maxSize)
{
    if(mPortConf.PortStatus){
        return mSerialPort->write(data,maxSize);
    }else{
        QMessageBox::warning(this,tr("错误"),tr("串口未打开!"));
        return -1;
    }
}

qint64 SerialPort::write(const char *data)
{
    if(mPortConf.PortStatus){
        return mSerialPort->write(data);
    }else{
        QMessageBox::warning(this,tr("错误"),tr("串口未打开!"));
        return -1;
    }
}

qint64 SerialPort::write(const QByteArray &byteArray)
{
    if(mPortConf.PortStatus){
        return mSerialPort->write(byteArray);
    }else{
        QMessageBox::warning(this,tr("错误"),tr("串口未打开!"));
        return -1;
    }
}

void SerialPort::UpdataPortName()
{
    ui->comboBox_SerialPort->clear();

    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        ui->comboBox_SerialPort->addItem(info.portName()
                                         + "("
                                         + info.description()
                                         + ")");
    }
}

void SerialPort::GetUserPortConf()
{
    int temp;
    QString name = ui->comboBox_SerialPort->currentText();

    mPortConf.PortName = name.left(name.indexOf('('));

    temp = ui->comboBox_SerialBuad->currentText().toInt();
    switch (temp) {
        case 1200:
            mPortConf.PortBaud = QSerialPort::Baud1200;
        break;
        case 2400:
            mPortConf.PortBaud = QSerialPort::Baud2400;
        break;
        case 4800:
            mPortConf.PortBaud = QSerialPort::Baud4800;
        break;
        case 9600:
            mPortConf.PortBaud = QSerialPort::Baud9600;
        break;
        case 19200:
            mPortConf.PortBaud = QSerialPort::Baud19200;
        break;
        case 38400:
            mPortConf.PortBaud = QSerialPort::Baud38400;
        break;
        case 57600:
            mPortConf.PortBaud = QSerialPort::Baud57600;
        break;
        case 115200:
            mPortConf.PortBaud = QSerialPort::Baud115200;
        break;
    }

    temp = ui->comboBox_SerialBitNums->currentText().toInt();
    switch (temp) {
        case 8:
            mPortConf.PortDataBits = QSerialPort::Data8;
        break;
    }

    temp = ui->comboBox_SerialStopBit->currentText().toInt();
    switch (temp) {
        case 1:
            mPortConf.PortStopBits = QSerialPort::OneStop;
        break;
        case 2:
            mPortConf.PortStopBits = QSerialPort::TwoStop;
        break;
        case 3:
            mPortConf.PortStopBits = QSerialPort::OneAndHalfStop;
        break;

    }

    temp = ui->comboBox_Parity->currentIndex();
    switch (temp) {
        case 0:
            mPortConf.PortParity = QSerialPort::NoParity;
        break;
        case 1:
            mPortConf.PortParity = QSerialPort::OddParity;
        break;
        case 2:
            mPortConf.PortParity = QSerialPort::EvenParity;
        break;
    }
}

void SerialPort::SwitchUIWidget(bool onff)
{
    ui->comboBox_SerialPort->setEnabled(onff);
    ui->comboBox_SerialBuad->setEnabled(onff);
    ui->comboBox_SerialBitNums->setEnabled(onff);
    ui->comboBox_Parity->setEnabled(onff);
    ui->comboBox_SerialStopBit->setEnabled(onff);
    ui->pushButton_SerialRefresh->setEnabled(onff);
}

void SerialPort::on_pushButton_SerialOpenPort_clicked()
{
    GetUserPortConf();
    if(mPortConf.PortStatus == false){
        mSerialPort->setPortName(mPortConf.PortName);
        mSerialPort->setBaudRate(mPortConf.PortBaud);
        mSerialPort->setDataBits(mPortConf.PortDataBits);
        mSerialPort->setFlowControl(QSerialPort::NoFlowControl);
        mSerialPort->setParity(mPortConf.PortParity);
        mSerialPort->setStopBits(mPortConf.PortStopBits);

        if(!mSerialPort->open(QIODevice::ReadWrite)){
            mPortConf.PortStatus = false;
            QMessageBox::warning(this,"Warning",tr("打不开这个端口，请刷新端口列表!"));
        }else{
            mPortConf.PortStatus = true;
            ui->pushButton_SerialOpenPort->setText(tr("关闭串口"));

            SwitchUIWidget(!mPortConf.PortStatus);
        }
    }else{
        mSerialPort->close();
        mPortConf.PortStatus = false;
        ui->pushButton_SerialOpenPort->setText(tr("打开串口"));

        SwitchUIWidget(!mPortConf.PortStatus);
    }
    emit portStatus(mPortConf.PortStatus);

}

void SerialPort::on_pushButton_SerialRefresh_clicked()
{
    UpdataPortName();
}

void SerialPort::EmitReadyRead()
{
    emit readyRead();
}
