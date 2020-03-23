#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H


#include <QThread>
#include <QTextCodec>

typedef struct SerialConf_tag{
    bool HexSend;
    bool HexRec;
    bool SendEnter;
    bool PausStatus;                    //暂停接收状态0:未暂停1:暂停
}SerialConf_t;


class SerialThread : public QThread
{
public:
    SerialThread();

    void PushMessage(QByteArray array,SerialConf_t conf);
    QString PopMessage(void);

protected:
    void run() override;

private:
    QByteArray RawByteArray;
    QString ProcessedByteArray;

    SerialConf_t mSerialConf;
};

#endif // SERIALTHREAD_H
