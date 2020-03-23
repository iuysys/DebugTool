#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QDebug>



MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    mCurrentWidgetIndex = 0;
    ui->tabWidget->setCurrentIndex(mCurrentWidgetIndex);
    ui->toolBox->setCurrentIndex(0);

    connect(ui->PortSerial,SIGNAL(readyRead()),this,SLOT(MessageProcess()));
    connect(ui->PortSerial,SIGNAL(portStatus(bool)),ui->SerialAssistant,SLOT(GetPortStatus(bool)));
    connect(ui->SerialAssistant,SIGNAL(lanuchData(const QByteArray &)),ui->PortSerial,SLOT(write(const QByteArray &)));

}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    ui->PortSerial->close();
}

void MainWidget::MessageProcess()
{
//    qDebug()<< ui->Port->readAll();
    QByteArray RawData = ui->PortSerial->readAll();

    switch (mCurrentWidgetIndex) {
        case 0:
            ui->SerialAssistant->addRawData(RawData);
        break;

        case 1:
            ui->RSSI->addRawData(RawData);
            if(ui->checkBox_SyncShow_RSSI->isChecked()){
                ui->SerialAssistant->addRawData(RawData);
            }
        break;

        case 2:
            ui->LogCaptureTool->pushData(RawData);

            if(ui->checkBox_SyncShow_LogCapture->isChecked()){
                ui->SerialAssistant->addRawData(RawData);
            }
        break;
    }
}

void MainWidget::on_tabWidget_currentChanged(int index)
{
    mCurrentWidgetIndex = index;
}


