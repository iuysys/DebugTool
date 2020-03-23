#include "serialthread.h"

SerialThread::SerialThread()
{

}

void SerialThread::PushMessage(QByteArray array, SerialConf_t conf)
{
    RawByteArray.clear();
    ProcessedByteArray.clear();

    RawByteArray.append(array);
    mSerialConf = conf;
}

QString SerialThread::PopMessage()
{
    return ProcessedByteArray;
}

void SerialThread::run()
{
    if(mSerialConf.HexRec){
        int i;
        QByteArray temp = RawByteArray.toHex();
        QByteArray hexTemp;
        for(i = 0;i < temp.size();i += 2){
            hexTemp.push_back(temp.mid(i,2));
            hexTemp.push_back(" ");
        }
        ProcessedByteArray = QString(hexTemp);
    }else{
        QTextCodec *tc = QTextCodec::codecForName("GBK");
        ProcessedByteArray = tc->toUnicode(RawByteArray);
    }
}
