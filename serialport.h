#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QWidget>
#include <QSerialPortInfo>
#include <QSerialPort>

namespace Ui {
class SerialPort;
}

class SerialPort : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPort(QWidget *parent = nullptr);
    ~SerialPort();
    QByteArray readAll();




signals:
    void readyRead();
    void portStatus(bool onff);

public slots:
    qint64 write(const char *data, qint64 maxSize);
    qint64 write(const char *data);
    qint64 write(const QByteArray &byteArray);

private slots:
    void on_pushButton_SerialOpenPort_clicked();

    void on_pushButton_SerialRefresh_clicked();

    void EmitReadyRead();

private:
    struct{
        QString PortName;
        QSerialPort::BaudRate PortBaud;
        QSerialPort::DataBits PortDataBits;
        QSerialPort::Parity PortParity;
        QSerialPort::StopBits PortStopBits;
        bool PortStatus;
    }mPortConf;

    QSerialPort *mSerialPort;

private:
    Ui::SerialPort *ui;
    void UpdataPortName(void);
    void GetUserPortConf(void);
    void SwitchUIWidget(bool onff);
};

#endif // SERIALPORT_H
