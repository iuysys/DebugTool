#ifndef RSSITHREAD_H
#define RSSITHREAD_H

#include <QThread>
#include <QVector>

/*
 * 结点名枚举
 */
typedef enum nodeName_tag{
    MasterNode  = 0,
    LeftNode,
    RightNode,
    TrunkNode,
    BumperNode,
    NodeMax
}NodeName_t;





class RSSIThread : public QThread
{
public:
    RSSIThread();

    void PushRawData(QByteArray arry);
    QVector<double> PopWaveData(NodeName_t node);
    void ThreadDataClear(void);

protected:
    void run() override;

protected:
    QString waveRawData;
    QVector<double> NodeRawYData[NodeMax];
    #if 0
    QStringList StrHeade = {
        "MASTER_NODE:",
        "FRONT_LEFT_NODE:",
        "FRONT_RIGHT_NODE:",
        "TRUNK_NODE:",
        "BUMPER_NODE:"
    };
    #else
    QStringList StrHeade = {
        "MASTER_NODE:",
        "LEFT_NODE:",
        "RIGHT_NODE:",
        "TRUNK_NODE:",
        "BUMPER_NODE:"
    };
    #endif

};

#endif // RSSITHREAD_H
