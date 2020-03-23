#include "logcapture.h"
#include "ui_logcapture.h"


#include <QDebug>
#include <QMessageBox>

LogCapture::LogCapture(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogCapture)
{
    ui->setupUi(this);
    dataModel = new QStandardItemModel();
    ui->tableView->setModel(dataModel);

    Start = false;

    dataModel->setColumnCount(4);
    dataModel->setHeaderData(0,Qt::Horizontal,tr("特征头"));
    dataModel->setHeaderData(1,Qt::Horizontal,tr("特征值"));
    dataModel->setHeaderData(2,Qt::Horizontal,tr("捕捉计数"));
    dataModel->setHeaderData(3,Qt::Horizontal,tr("捕捉时间戳"));
//    ui->tableView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3,QHeaderView::ResizeToContents);

    addItemDlg = new TextInfoInputDlg(this);
    addItemDlg->setModal(true);


    mThread = new LogCaptureThread();
    connect(addItemDlg,SIGNAL(text(const QString)),this,SLOT(addItem(const QString)));
    connect(addItemDlg,SIGNAL(text(const QString)),mThread,SLOT(addItem(const QString)));
    connect(mThread,SIGNAL(dataChanged(const LatestRowData_t)),this,SLOT(getLatestRowData(const LatestRowData_t)));

    readConfIni(".\\iniFile\\LogCapture.ini");

}

LogCapture::~LogCapture()
{
    delete ui;
}

void LogCapture::pushData(const QByteArray arry)
{
    if(Start){
        mThread->pushRawData(arry);
        mThread->start();
    }
}


void LogCapture::on_pushButton_addItem_clicked()
{
    addItemDlg->show();
}

void LogCapture::addItem(const QString str)
{
    LogCaptureRowData_t temp;

    FeatureList.insert(0,str);

    temp.feature = new QStandardItem(str);
    temp.value.clear();
    temp.cnt = 0;
    temp.timeStamp.clear();

    temp.value.insert(0,"NULL");
    temp.timeStamp.insert(0,"00:00:00.000");

    RowRawData.insert(0,temp);

    dataModel->insertRow(0,RowRawData[0].feature);
    dataModel->setData(dataModel->index(0,1),RowRawData[0].value[0]);
    dataModel->setData(dataModel->index(0,2),RowRawData[0].cnt);
    dataModel->setData(dataModel->index(0,3),RowRawData[0].timeStamp[0]);
}

void LogCapture::getLatestRowData(const LatestRowData_t rowData)
{
//    qDebug()<<"getLatestRowData";
    int index = rowData.index;
    int tableRow = dataModel->indexFromItem(RowRawData[index].feature).row();
    RowRawData[index].cnt += 1;
    RowRawData[index].value.insert(0,rowData.value);
    RowRawData[index].timeStamp.insert(0,rowData.timeStamp);

    dataModel->setData(dataModel->index(tableRow,1),RowRawData[index].value[0]);
    dataModel->setData(dataModel->index(tableRow,2),RowRawData[index].cnt);
    dataModel->setData(dataModel->index(tableRow,3),RowRawData[index].timeStamp[0]);
}

void LogCapture::on_pushButton_deleteItem_clicked()
{
    int row = ui->tableView->currentIndex().row();
    int index;
    QString str = dataModel->data(dataModel->index(row,0)).toString();
    for(index = 0; index < FeatureList.size();index++){
        if(str == FeatureList[index]){
            break;
        }
    }
    FeatureList.removeAt(index);
    RowRawData.removeAt(index);
    mThread->deleteItem(index);
    dataModel->removeRow(row);
}

void LogCapture::on_checkBox_Start_clicked(bool checked)
{
    Start = checked;

    if(Start){
        ui->checkBox_Start->setText(tr("启动"));
        writeIniFile(".\\iniFile\\LogCapture.ini");
    }else{
        ui->checkBox_Start->setText(tr("停止"));
    }
    EnableUIButton(!Start);
}

void LogCapture::EnableUIButton(bool onff)
{
    ui->pushButton_addItem->setEnabled(onff);
    ui->pushButton_deleteItem->setEnabled(onff);
    ui->pushButton_exportConf->setEnabled(onff);
    ui->pushButton_importConf->setEnabled(onff);
    ui->pushButton_clearHistory->setEnabled(onff);
    ui->pushButton_clearSetting->setEnabled(onff);
}

void LogCapture::clearHistory()
{
    if(!FeatureList.isEmpty()){
        for(int i = 0; i < FeatureList.size(); i++){
            RowRawData[i].cnt = 0;
            RowRawData[i].value.clear();
            RowRawData[i].timeStamp.clear();
            RowRawData[i].value.insert(0,"NULL");
            RowRawData[i].timeStamp.insert(0,"00:00:00.000");

            dataModel->setData(dataModel->index(i,1),RowRawData[i].value[0]);
            dataModel->setData(dataModel->index(i,2),RowRawData[i].cnt);
            dataModel->setData(dataModel->index(i,3),RowRawData[i].timeStamp[0]);
        }
    }
}

void LogCapture::clearUserConf()
{
    RowRawData.clear();
    FeatureList.clear();
    dataModel->removeRows(0,dataModel->rowCount());
    mThread->clear();
}

void LogCapture::readConfIni(const QString path)
{
    QFile files(path);

    if(files.exists()){
        QSettings * setting = new QSettings(path,QSettings::IniFormat);
        int size = setting->value("Featrue/size").toInt();
        QString str;
        for(int i = 0; i < size; i++){
            str = setting->value(QString("Featrue/value%1").arg(i)).toString();
            addItem(str);
            mThread->addItem(str);
        }
        delete setting;
    }else{
        files.open(QIODevice::WriteOnly);
        files.close();
    }
}

void LogCapture::writeIniFile(const QString path)
{
    QFile file(path);

    if(!file.exists()){
        file.open(QIODevice::WriteOnly);
        file.close();
    }

    QSettings *setting = new QSettings(path,QSettings::IniFormat);

    setting->clear();
    setting->setValue("Featrue/size",FeatureList.size());
    for(int i = 0; i < FeatureList.size(); i++){
        setting->setValue(QString("Featrue/value%1").arg(i),FeatureList[i]);
    }

    delete setting;

}

void LogCapture::on_pushButton_clearHistory_clicked()
{
    clearHistory();
}

void LogCapture::on_pushButton_exportConf_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("导出配置文件"),
                                                    "./",
                                                    tr("配置文件(*.ini)"));
    if(!fileName.isEmpty()){
        writeIniFile(fileName);
    }

}

void LogCapture::on_pushButton_importConf_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr(""),
                                                    "./",
                                                    tr("配置文件(*.ini)"));
    if(!fileName.isEmpty()){
        clearUserConf();                                    //确认选择了文件再清除
        readConfIni(fileName);
    }

}

void LogCapture::on_pushButton_clearSetting_clicked()
{
    int select = QMessageBox::question(this,tr("清空配置?"),tr("确定回到解放前?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(select == QMessageBox::Yes){
        clearUserConf();
    }
}

void LogCapture::on_pushButton_saveSetting_clicked()
{
    writeIniFile(".\\iniFile\\LogCapture.ini");
}
