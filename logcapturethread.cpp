#include "logcapturethread.h"
#include <QDebug>
#include <QTime>
#include <QMetaType>

LogCaptureThread::LogCaptureThread()
{
    qRegisterMetaType<LatestRowData_t>("LatestRowData_t");
    clear();
}

void LogCaptureThread::pushRawData(const QByteArray arry)
{
    RawData.append(QString::fromLocal8Bit(arry));
}

void LogCaptureThread::clear()
{
    RawData.clear();
    FeatureList.clear();
}

void LogCaptureThread::addItem(const QString str)
{
    FeatureList.insert(0,str);
}

void LogCaptureThread::deleteItem(const int index)
{
    FeatureList.removeAt(index);
}

void LogCaptureThread::run()
{
//    qDebug()<<"LogCaptureThread Runing";
    QString lastLine;
    QString timeStamp;
    int index,indexCR;
    LatestRowData_t CaptureData;

    index = RawData.lastIndexOf('\n');                  //优化考虑文件尾只存在\r的情况,分隔仍以\n分隔
    indexCR = RawData.lastIndexOf('\r');
    index = index < indexCR ? indexCR : index;

    if(index >= 0 && index <= (RawData.size() - 1)){

       QStringList strList = RawData.left(index + 1).split('\n',QString::SkipEmptyParts);

       if(index < (RawData.size() - 1)){
          lastLine = RawData.right(RawData.size() - index);
          RawData.clear();
          RawData.append(lastLine);
       }else{
           RawData.clear();
       }


       foreach(QString line,strList){
           qDebug()<< line;
           for (int j = 0;j < FeatureList.size();j++) {
               if(line.indexOf(FeatureList[j]) >= 0){
                   CaptureData.index = j;
                   timeStamp = QTime::currentTime().toString("hh:mm:ss.zzz");
                   CaptureData.timeStamp = timeStamp;
                   if(FeatureList[j] != line){
                       CaptureData.value = line.right(line.size() - FeatureList[j].size());
                   }
//                   qDebug()<<"LogCaptureThread emit data";
                   emit dataChanged(CaptureData);
                   break;
               }
           }
       }
    }
}


