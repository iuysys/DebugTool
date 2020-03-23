#include "serialtool.h"
#include "ui_serialtool.h"

#include <QDebug>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>

SerialTool::SerialTool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialTool)
{
    ui->setupUi(this);

    mTxCnt = 0;
    mRxCnt = 0;
    portStatus = false;
    mSerialConf = {false,false,false,false};
    SerialAutoSendStruct = {0,0,0};

    mThread = new SerialThread();
    mTimer = new QTimer();
    mTimer->start(500);

    connect(mThread,SIGNAL(finished()),this,SLOT(TextBrowserShowMessage()));
    connect(mTimer,SIGNAL(timeout()),this,SLOT(TimeOutService()));

    connect(ui->checkBox_SerialHexRec,SIGNAL(clicked()),this,SLOT(GetSerialConf()));
    connect(ui->checkBox_SerialHexSend,SIGNAL(clicked()),this,SLOT(GetSerialConf()));
    connect(ui->checkBox_SerialAutoSend,SIGNAL(clicked()),this,SLOT(GetSerialConf()));
    connect(ui->checkBox_SerialSendEnter,SIGNAL(clicked()),this,SLOT(GetSerialConf()));

}

SerialTool::~SerialTool()
{
    delete ui;
}

int SerialTool::addRawData(const QByteArray arry)
{
    qDebug()<<"SerialAssistant addRawData \n";
    if(!mSerialConf.PausStatus){
        mRxCnt += arry.size();
        mThread->PushMessage(arry,mSerialConf);
        mThread->start();
    }
    return 0;
}


void SerialTool::TextBrowserShowMessage()
{
    if(!mSerialConf.PausStatus){
        QString temp = mThread->PopMessage();

        if(ui->checkBox_KeepLast->isChecked()){
            ui->textBrowser_Serial->moveCursor(QTextCursor::End);
        }else{
            //光标可能被移动，方式1：需要先移动到末尾才能添加数据，方式2：先储存，再添加
        }


//        QString time = QDateTime::currentDateTime().toString("[MM-dd HH:mm:ss ->]");
//        if(ui->checkBox_TimeStamp->isChecked()){
//            //在\r\n后面添加时间戳，这就得拆包
//        }

        ui->textBrowser_Serial->insertPlainText(temp);
    }
}

void SerialTool::GetSerialConf()
{
    mSerialConf.HexRec = ui->checkBox_SerialHexRec->isChecked();
    mSerialConf.HexSend = ui->checkBox_SerialHexSend->isChecked();
    mSerialConf.SendEnter = ui->checkBox_SerialSendEnter->isChecked();
    if(ui->checkBox_SerialAutoSend->isChecked()){
        SerialAutoSendStruct.AutoSendCycle = ui->lineEdit_SerialAutoSendTime->text().toInt();
        SerialAutoSendStruct.time = SerialAutoSendStruct.AutoSendCycle;
        ui->lineEdit_SerialAutoSendTime->setEnabled(false);
        ui->lineEdit_SerialSend->setEnabled(false);

        if(SerialAutoSendStruct.AutoSendCycle % 1000 == 0){
            mTimer->start(500);
        }else if(SerialAutoSendStruct.AutoSendCycle % 100 == 0){
            mTimer->start(100);
        }else{
            mTimer->start(10);
        }
    }else{
        SerialAutoSendStruct.AutoSendCycle = 0;
        ui->lineEdit_SerialAutoSendTime->setEnabled(true);
        ui->lineEdit_SerialSend->setEnabled(true);
        mTimer->start(500);
    }
    SerialAutoSendStruct.TimeOutInteval = mTimer->interval();
    //    qDebug()<<mSerialConf.AutoSendCycle;
}

void SerialTool::GetPortStatus(bool onff)
{
    portStatus = onff;
}


void SerialTool::on_textBrowser_Serial_textChanged()
{
    if(ui->checkBox_KeepLast->isChecked()){
        ui->textBrowser_Serial->moveCursor(QTextCursor::End);
    }

}

void SerialTool::on_pushButton_SerialSend_clicked()
{
    if(portStatus){
        QByteArray hexArray;
        bool ok;
        QString temp = ui->lineEdit_SerialSend->text().simplified();            //去除前后空格
        if(mSerialConf.SendEnter){
            temp.append("\r\n");
        }
        if(mSerialConf.HexSend){
            QStringList hexList = temp.split(' ');
            foreach(QString str,hexList){
                hexArray.append(static_cast<char>(str.toInt(&ok,16)));
            }
            emit lanuchData(hexArray);
            mTxCnt += hexArray.size();
        }else{
            emit lanuchData(temp.toUtf8());          //发送中文还要转码
            mTxCnt += temp.size();
        }
    }else{
        QMessageBox::warning(this,"Warning",tr("串口未打开!"));
    }

}

void SerialTool::TimeOutService()
{
//    qDebug()<<"mTxCnt:"<<mTxCnt<<"\n";
    ui->label_SerialTxCnt->setNum(static_cast<double>(mTxCnt));
    ui->label_SerialRxCnt->setNum(static_cast<double>(mRxCnt));

    if(SerialAutoSendStruct.AutoSendCycle && portStatus){
        SerialAutoSendStruct.time -= SerialAutoSendStruct.TimeOutInteval;
        if(SerialAutoSendStruct.time <= 0){
            SerialAutoSendStruct.time = SerialAutoSendStruct.AutoSendCycle;
            on_pushButton_SerialSend_clicked();
        }
    }
}

void SerialTool::FindText(const FindStruct_t * fs)
{
    QTextDocument::FindFlags options;

    if(fs->WholeWords){
        options = QTextDocument::FindWholeWords;
    }
    if(fs->CaseSensitively){
        options |= QTextDocument::FindCaseSensitively;
    }
    if(fs->FindBackward){
        options |= QTextDocument::FindBackward;
    }

    if(ui->textBrowser_Serial->find(fs->FindStr,options)){
        QPalette palette = ui->textBrowser_Serial->palette();
        palette.setColor(QPalette::Highlight,palette.color(QPalette::Active,QPalette::Mid));
        ui->textBrowser_Serial->setPalette(palette);
    }else{
        if(fs->FindBackward){
            ui->textBrowser_Serial->moveCursor(QTextCursor::Start);
        }else{
            ui->textBrowser_Serial->moveCursor(QTextCursor::End);
        }
    }
}


void SerialTool::on_checkBox_SerialPaus_clicked(bool checked)
{
    if(checked){
        mSerialConf.PausStatus = false;
        ui->checkBox_SerialPaus->setText(tr("开始"));
    }else{
        mSerialConf.PausStatus = true;
        ui->checkBox_SerialPaus->setText(tr("暂停"));
    }
}

void SerialTool::on_pushButton_SerialSaveRec_clicked()
{
    QString temp = ui->textBrowser_Serial->toPlainText();
    if(temp.isEmpty()){
        QMessageBox::warning(this,"Warning",tr("没数据啊，保存个毛线!"));
        return;
    }

    QString timeStr = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString saveName =QFileDialog::getSaveFileName(this,tr("保存接收"),timeStr,tr("Text File(*.txt)"));
    qDebug()<< saveName;
    if(!saveName.isEmpty()){
        QFile txtFile(saveName);
        if(!txtFile.open(QIODevice::WriteOnly)){
            QMessageBox::warning(this,"Warning",tr("文件创建失败!"));
            return;
        }else{
            QTextStream txtOut(&txtFile);

            temp.replace("\n","\r\n");
            txtOut<<temp<<endl;
            txtOut.flush();
            txtFile.close();
        }
    }
}


void SerialTool::on_pushButton_SerialClearSend_clicked()
{
    mTxCnt = 0;
    ui->lineEdit_SerialSend->setText("");
}

void SerialTool::on_pushButton_SerialClearRec_clicked()
{
    mRxCnt = 0;
    ui->textBrowser_Serial->setText("");                    //用clear无效，待解决
}

void SerialTool::on_pushButton_FindBackward_clicked()
{
    FindStruct.FindStr = ui->lineEdit_find->text();
    if(FindStruct.FindStr.isEmpty())
        return;

    FindStruct.CaseSensitively = ui->checkBox_CaseSensitively->isChecked();
    FindStruct.WholeWords = ui->checkBox_WholeWords->isChecked();
    FindStruct.FindBackward = true;
    FindText(&FindStruct);
}

void SerialTool::on_pushButton_Forwards_clicked()
{
    FindStruct.FindStr = ui->lineEdit_find->text();
    if(FindStruct.FindStr.isEmpty())
        return;

    FindStruct.CaseSensitively = ui->checkBox_CaseSensitively->isChecked();
    FindStruct.WholeWords = ui->checkBox_WholeWords->isChecked();
    FindStruct.FindBackward = false;
    FindText(&FindStruct);
}
