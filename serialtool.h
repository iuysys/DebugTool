#ifndef SERIALTOOL_H
#define SERIALTOOL_H

#include <QWidget>
#include "serialthread.h"
#include <QTimer>

namespace Ui {
class SerialTool;
}

class SerialTool : public QWidget
{
    Q_OBJECT

public:
    explicit SerialTool(QWidget *parent = nullptr);
    ~SerialTool();
    int addRawData(const QByteArray arry);
signals:
    void lanuchData(const QByteArray &arry);



private:
    Ui::SerialTool *ui;
    SerialThread * mThread;
    SerialConf_t mSerialConf;

    struct{
        int AutoSendCycle;
        int TimeOutInteval;
        int time;
    }SerialAutoSendStruct;

    typedef struct{
        bool CaseSensitively;
        bool WholeWords;
        bool FindBackward;
        QString FindStr;
    }FindStruct_t;

    FindStruct_t FindStruct;

    bool portStatus;
    QTimer *mTimer;

    qint64 mTxCnt;
    qint64 mRxCnt;

    void FindText(const FindStruct_t * fs);




private slots:
    void TextBrowserShowMessage();

    void GetSerialConf(void);

    void GetPortStatus(bool onff);

    void on_textBrowser_Serial_textChanged();

    void on_pushButton_SerialSend_clicked();

    void TimeOutService();

    void on_checkBox_SerialPaus_clicked(bool checked);

    void on_pushButton_SerialSaveRec_clicked();

    void on_pushButton_SerialClearSend_clicked();

    void on_pushButton_SerialClearRec_clicked();

    void on_pushButton_FindBackward_clicked();

    void on_pushButton_Forwards_clicked();
};

#endif // SERIALTOOL_H
