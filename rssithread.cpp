#include "rssithread.h"
#include <QDebug>

/*
 *  12.30待增加  处理数据包时，以QVecctor<QByteArray> 返回每一次采集每个结点包含的有效数据个数
 *  测试断点划线逻辑
 */


RSSIThread::RSSIThread()
{
    ThreadDataClear();
}

void RSSIThread::PushRawData(QByteArray arry)
{
    waveRawData.append(QString::fromLocal8Bit(arry));
}

QVector<double> RSSIThread::PopWaveData(NodeName_t node)
{

    return NodeRawYData[node];
}

void RSSIThread::ThreadDataClear()
{
    waveRawData.clear();
    for(int i = 0; i < NodeMax;i++){
        NodeRawYData[i].clear();
    }
}



void RSSIThread::run()
{
    bool ok;
    qint8 num;
    QString lastLine;
    int index,indexCR;

    index = waveRawData.lastIndexOf('\n');                  //优化考虑文件尾只存在\r的情况,分隔仍以\n分隔
    indexCR = waveRawData.lastIndexOf('\r');
    index = index < indexCR ? indexCR : index;

    if(index >= 0 && index <= (waveRawData.size() - 1)){

       QStringList strList = waveRawData.left(index + 1).split('\n',QString::SkipEmptyParts);

       if(index < (waveRawData.size() - 1)){
          lastLine = waveRawData.right(waveRawData.size() - index);
          waveRawData.clear();
          waveRawData.append(lastLine);
       }else{
           waveRawData.clear();
       }

       foreach(QString line,strList){
            //可加入去除非字符
           for (int j = 0;j < NodeMax;j++) {
               if(line.indexOf(StrHeade[j]) >= 0){
                   QStringList list = line.split(' ',QString::SkipEmptyParts);
                   foreach(QString s,list){     //可加入处理异常长度
                       s = s.trimmed();         //去除末尾的控制字符
                       if(s.size() == 2){
                           num = qint8(s.toUInt(&ok,16));
                           NodeRawYData[j].append(num);
                       }
                   }
                   break;
               }
           }
       }
    }
}
