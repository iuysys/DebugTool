#ifndef LOGCAPTURE_H
#define LOGCAPTURE_H

#include <QWidget>
#include <QStandardItemModel>
#include "logcapturethread.h"
#include "textinfoinputdlg.h"
#include <QFileDialog>
#include <QSettings>

namespace Ui {
class LogCapture;
}


class LogCapture : public QWidget
{
    Q_OBJECT

public:
    explicit LogCapture(QWidget *parent = nullptr);
    ~LogCapture();

    void pushData(const QByteArray arry);    

private slots:

    void on_pushButton_addItem_clicked();

    void addItem(const QString str);

    void getLatestRowData(const LatestRowData_t rowData);

    void on_pushButton_deleteItem_clicked();

    void on_checkBox_Start_clicked(bool checked);

    void on_pushButton_clearHistory_clicked();

    void on_pushButton_exportConf_clicked();

    void on_pushButton_importConf_clicked();

    void on_pushButton_clearSetting_clicked();

    void on_pushButton_saveSetting_clicked();

private:

    typedef struct{
        QStandardItem *feature;
        QStringList value;
        quint64 cnt;
        QStringList timeStamp;
    }LogCaptureRowData_t;

    Ui::LogCapture *ui;
    QStandardItemModel * dataModel;
    QList<LogCaptureRowData_t> RowRawData;
    QStringList FeatureList;

    LogCaptureThread *mThread;
    TextInfoInputDlg *addItemDlg;

    bool Start;

    void EnableUIButton(bool onff);
    void clearHistory(void);
    void clearUserConf(void);

    void readConfIni(const QString path);

    void writeIniFile(const QString path);
};

#endif // LOGCAPTURE_H
