#ifndef LOGCAPTURETHREAD_H
#define LOGCAPTURETHREAD_H

#include <QThread>
#include <QStandardItemModel>





typedef struct{
    int index;
    QString value;
    QString timeStamp;
}LatestRowData_t;


class LogCaptureThread : public QThread
{
    Q_OBJECT
public:
    LogCaptureThread();
    void pushRawData(const QByteArray arry);
    void clear(void);
signals:
    void dataChanged(const LatestRowData_t rowData);

public slots:
    void addItem(const QString str);
    void deleteItem(const int index);

protected:
    void run() override;

private:
//    QStandardItemModel *RelationModel;
//    QList<LogCaptureRowData_t> RowRawData;
    QStringList FeatureList;
    QString RawData;
};

#endif // LOGCAPTURETHREAD_H
