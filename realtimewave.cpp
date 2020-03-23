#include "realtimewave.h"
#include "ui_realtimewave.h"
#include <QtXlsx>
#include <QMessageBox>
#include <QFileDialog>

RealTimeWave::RealTimeWave(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RealTimeWave)
{
    ui->setupUi(this);
    mRSSIConf = {false,true,true,true,true,true,true};

    mScaleConf = {false,false,0.0,255.0,255.0,-100.0,0.0,100.0};

    mThread = new RSSIThread();
    connect(mThread,SIGNAL(finished()),this,SLOT(ShowWave()));

    mCanvas = new QwtPlotCanvas();
    mCanvas->setBorderRadius(4);
    ui->qwtPlot->setCanvas(mCanvas);
    ui->qwtPlot->setCanvasBackground(QColor("white"));
    //坐标轴设置
    ui->qwtPlot->enableAxis(QwtPlot::yLeft,false);              //关闭Y左轴
    ui->qwtPlot->enableAxis(QwtPlot::yRight,true);
    ui->qwtPlot->setAxisAutoScale(QwtPlot::yRight,true);
    //设置x,y坐标尺度
    ui->qwtPlot->setAxisScale(QwtPlot::yRight,-100,0,10);
    ui->qwtPlot->setAxisScale(QwtPlot::xBottom,0,255);
    //网格设置
    mWaveGrid = new QwtPlotGrid();
    mWaveGrid->enableXMin(true);
    mWaveGrid->enableYMin(true);
    mWaveGrid->setYAxis(QwtPlot::yRight);                       //关联网格和Y右轴,默认是左轴
    mWaveGrid->setMajorPen(QPen(Qt::white,0,Qt::SolidLine));
    mWaveGrid->setMinorPen(QPen(Qt::gray,0,Qt::DotLine));
    mWaveGrid->attach(ui->qwtPlot);
    //平移
    mPanner = new QwtPlotPanner(ui->qwtPlot->canvas());
    mPanner->setMouseButton(Qt::RightButton);
    ui->qwtPlot->setAutoReplot(true);
    //缩放
    mZoomer = new QwtPlotZoomer(ui->qwtPlot->canvas());
    mZoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton,Qt::ControlModifier);
    mZoomer->setZoomBase(true);

    mMagnifier = new QwtPlotMagnifier(ui->qwtPlot->canvas());                 //默认的滑轮及右键缩放功能图形的整体缩放
    mMagnifier->setMouseButton(Qt::RightButton,Qt::ControlModifier);
    //生成波形实例
    for(int i = 0;i < NodeMax;i++){
        mCurve[i] = new QwtPlotCurve();
        mCurve[i]->setYAxis(QwtPlot::yRight);                               //关联Y右轴
        mCurve[i]->setCurveAttribute(QwtPlotCurve::Fitted,false);           //关闭滤波
        mCurve[i]->attach(ui->qwtPlot);
    }
    //波形颜色
    mCurve[LeftNode]->setPen(QColor(255, 0, 0),2,Qt::SolidLine);
    mCurve[RightNode]->setPen(QColor(0, 255, 0),2,Qt::SolidLine);
    mCurve[MasterNode]->setPen(QColor(0, 0, 255),2,Qt::SolidLine);
    mCurve[TrunkNode]->setPen(QColor(255, 255, 0),2,Qt::SolidLine);
    mCurve[BumperNode]->setPen(QColor(0, 255, 255),2,Qt::SolidLine);

    connect(ui->checkBox_RSSIMasterNode,SIGNAL(clicked(bool)),this,SLOT(GetUserUIConf()));
    connect(ui->checkBox_RSSILeftNode,SIGNAL(clicked(bool)),this,SLOT(GetUserUIConf()));
    connect(ui->checkBox_RSSIRightNode,SIGNAL(clicked(bool)),this,SLOT(GetUserUIConf()));
    connect(ui->checkBox_RSSITrunkNode,SIGNAL(clicked(bool)),this,SLOT(GetUserUIConf()));
    connect(ui->checkBox_RSSIBumperNode,SIGNAL(clicked(bool)),this,SLOT(GetUserUIConf()));

    connect(ui->checkBox_RSSIWaveFollow,SIGNAL(clicked(bool)),this,SLOT(GetUserUIConf()));
    connect(ui->pushButton_RSSIClearWave,SIGNAL(clicked()),this,SLOT(WaveDataClear()));
}

RealTimeWave::~RealTimeWave()
{
    delete ui;
}

void RealTimeWave::addRawData(const QByteArray arry)
{
    qDebug()<<"RSSI addRawData \n";
    if(mRSSIConf.onff){
        mThread->PushRawData(arry);
        mThread->start();
    }

}

void RealTimeWave::on_checkBox_RSSIStart_clicked(bool checked)
{
    mRSSIConf.onff = checked;

    if(mRSSIConf.onff){
        WaveDataClear();
        ui->checkBox_RSSIStart->setText(tr("ON"));
    }else{

        ui->checkBox_RSSIStart->setText(tr("OFF"));
    }
}

void RealTimeWave::ShowWave()
{  
    for(int i = 0; i < NodeMax; i++){
        waveYData[i] = mThread->PopWaveData(NodeName_t(i));
    }

    int i;
    int len = waveYData[0].size();

    for(i = 1; i < NodeMax;i++){
        if(len < waveYData[i].size()){
            len = waveYData[i].size();
        }
    }

    for(i = waveXData.size(); i < len; i++){
        waveXData.push_back(i);
    }

    for(i = 0;i < NodeMax;i++){
        mCurve[i]->setSamples(waveXData,waveYData[i]);
    }

    if(mRSSIConf.WaveTrack){                                                       //开启自动追踪

        if(waveXData.size() > mScaleConf.xAxisSpan){
            ui->qwtPlot->setAxisScale(QwtPlot::xBottom,waveXData.size() - mScaleConf.xAxisSpan,waveXData.size());
        }
    }

    ui->qwtPlot->replot();
}

void RealTimeWave::GetUserUIConf()
{
    mRSSIConf.ShowMasterNode = ui->checkBox_RSSIMasterNode->isChecked();
    mRSSIConf.ShowLeftNode = ui->checkBox_RSSILeftNode->isChecked();
    mRSSIConf.ShowRightNode = ui->checkBox_RSSIRightNode->isChecked();
    mRSSIConf.ShowTrunkNode = ui->checkBox_RSSITrunkNode->isChecked();
    mRSSIConf.ShowBumperNode = ui->checkBox_RSSIBumperNode->isChecked();

//    mRSSIConf.onff = ui->checkBox_RSSIStart->isChecked();
    mRSSIConf.WaveTrack = ui->checkBox_RSSIWaveFollow->isChecked();

    mCurve[MasterNode]->setVisible(mRSSIConf.ShowMasterNode);
    mCurve[LeftNode]->setVisible(mRSSIConf.ShowLeftNode);
    mCurve[RightNode]->setVisible(mRSSIConf.ShowRightNode);
    mCurve[TrunkNode]->setVisible(mRSSIConf.ShowTrunkNode);
    mCurve[BumperNode]->setVisible(mRSSIConf.ShowBumperNode);

    ui->qwtPlot->replot();
}

void RealTimeWave::WaveDataClear()
{
    int k;
    for(k = 0; k < NodeMax;k++){
        waveYData[k].clear();
    }
    waveXData.clear();
    mThread->ThreadDataClear();

    //清空波形数据
    for(k = 0;k < NodeMax;k++){
        mCurve[k]->setSamples(waveXData,waveYData[k]);
    }
    ui->qwtPlot->setAxisScale(QwtPlot::xBottom,0,255);
    ui->qwtPlot->setAxisScale(QwtPlot::yRight,-100,0);
    ui->qwtPlot->replot();
}


void RealTimeWave::on_pushButton_RSSIExportWave_clicked()
{
    int i = 0;
    for(i = 0; i < NodeMax; i++){
        if(waveYData[i].isEmpty())
            continue;
        else
            break;
    }

    if(i == NodeMax){

        QMessageBox::warning(this,"Warning",tr("波形数据为空!"));
        return;
    }

    QString timeStr = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString saveName =QFileDialog::getSaveFileName(this,tr("保存波形数据"),"Wave_" + timeStr,tr("Excel File(*.xlsx)"));           //增加默认文件名前缀Wave

    QXlsx::Document excel(saveName);
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

    for(i = 0; i < NodeMax; i++){
        excel.write(i * 2 + 1,1,StrHeade[i]);
        for(int j = 0; j < waveYData[i].size(); j++){
            excel.write((i + 1) * 2,j + 1,waveYData[i].at(j));
        }
    }

    excel.save();
}

void RealTimeWave::on_pushButton_RSSILoadWave_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("导入波形文件"),".",tr("Excel File(*.xlsx)"));
    if(!fileName.isEmpty()){
        qDebug()<<"打开文件"<<fileName;
        QXlsx::Document excel(fileName);
        bool ok;
        int i = 0,j = 0;
        double len = 0;

        WaveDataClear();

        for(i = 0; i < NodeMax; i++){
            qDebug()<<"Node:"<<i;
            while(excel.read((i + 1) * 2,j + 1).isValid()){
                waveYData[i].push_back(excel.read((i + 1) * 2,j + 1).toInt(&ok));
                qDebug()<<waveYData[i].at(j);
                j++;
            }
            len = len < j ? j : len;
            j = 0;
        }
        qDebug()<<"len = "<<len;
        for(double i = 0; i < len ; i++){
            waveXData.push_back(i);
        }

        for(int k = 0;k < NodeMax;k++){
            mCurve[k]->setSamples(waveXData,waveYData[k]);
        }
        ui->qwtPlot->replot();
    }
}

void RealTimeWave::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Shift){
//        qDebug()<< "shift 按下";
        mScaleConf.KeyShiftSt = true;
    }else if(ev->key() == Qt::Key_Control){
//        qDebug()<< "ctrl 按下";
        mScaleConf.KeyCtrlSt = true;
    }
}

void RealTimeWave::keyReleaseEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Shift){
//        qDebug()<< "shift 释放";
        mScaleConf.KeyShiftSt = false;

    }else if(ev->key() == Qt::Key_Control){
//        qDebug()<< "ctrl 释放";
        mScaleConf.KeyCtrlSt = false;
    }
}

void RealTimeWave::wheelEvent(QWheelEvent *event)
{
    qDebug()<< "滚轮事件"<<event->delta();

    if(mScaleConf.KeyCtrlSt){

        int delta = event->delta() / 12;
        delta =  (delta > 0 && delta * 2 >= int(mScaleConf.xAxisSpan)) ? 0 : delta;

        mScaleConf.xAxisStart = ui->qwtPlot->axisScaleDiv(QwtPlot::xBottom).lowerBound() + delta;
        mScaleConf.xAxisEnd = ui->qwtPlot->axisScaleDiv(QwtPlot::xBottom).upperBound() - delta;

//        qDebug()<<mScaleConf.xAxisStart<<mScaleConf.xAxisEnd<<mScaleConf.xAxisSpan<<delta;

        ui->qwtPlot->setAxisScale(QwtPlot::xBottom,mScaleConf.xAxisStart,mScaleConf.xAxisEnd);
        ui->qwtPlot->replot();

        mScaleConf.xAxisSpan = fabs(ui->qwtPlot->axisScaleDiv(QwtPlot::xBottom).range());
    }

    if(mScaleConf.KeyShiftSt){ 
        int delta = event->delta() / 24;
        delta =  (delta > 0 && delta * 2 >= int(mScaleConf.yAxisSpan)) ? 0 : delta;

        mScaleConf.yAxisStart = ui->qwtPlot->axisScaleDiv(QwtPlot::yRight).lowerBound() + delta;
        mScaleConf.yAxisEnd = ui->qwtPlot->axisScaleDiv(QwtPlot::yRight).upperBound() - delta;

//        qDebug()<<mScaleConf.yAxisStart<<mScaleConf.yAxisEnd<<mScaleConf.yAxisSpan<<delta;

        ui->qwtPlot->setAxisScale(QwtPlot::yRight,mScaleConf.yAxisStart,mScaleConf.yAxisEnd);
        ui->qwtPlot->replot();

        mScaleConf.yAxisSpan = fabs(ui->qwtPlot->axisScaleDiv(QwtPlot::yRight).range());
    }
}



