#ifndef REALTIMEWAVE_H
#define REALTIMEWAVE_H

#include <QWidget>
#include "qwt_plot_grid.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_panner.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_zoomer.h"
#include "qwt_plot_magnifier.h"

#include "rssithread.h"

namespace Ui {
class RealTimeWave;
}

class RealTimeWave : public QWidget
{
    Q_OBJECT

public:
    explicit RealTimeWave(QWidget *parent = nullptr);
    ~RealTimeWave();

    void addRawData(const QByteArray arry);

private slots:
    void on_checkBox_RSSIStart_clicked(bool checked);

    void ShowWave();

    void GetUserUIConf(void);

    void WaveDataClear(void);

    void on_pushButton_RSSIExportWave_clicked();

    void on_pushButton_RSSILoadWave_clicked();

protected:
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);
    void wheelEvent(QWheelEvent *event);

private:


private:
    Ui::RealTimeWave *ui;
    RSSIThread *mThread;
    struct{
        bool onff;

        bool ShowMasterNode;
        bool ShowLeftNode;
        bool ShowRightNode;
        bool ShowTrunkNode;
        bool ShowBumperNode;

        bool WaveTrack;

    }mRSSIConf;

    struct{
        bool KeyShiftSt;                            //预留为单向缩放
        bool KeyCtrlSt;                             //预留为单向缩放

        double xAxisStart;
        double xAxisEnd;
        double xAxisSpan;

        double yAxisStart;
        double yAxisEnd;
        double yAxisSpan;
    }mScaleConf;


    QwtPlotGrid *mWaveGrid;
    QwtPlotCanvas *mCanvas;
    QwtPlotPanner *mPanner;
    QwtPlotZoomer *mZoomer;
    QwtPlotMagnifier *mMagnifier;
    QwtPlotCurve *mCurve[NodeMax];               //波形

    QVector<double> waveXData;                   //通道X值储存
    QVector<double> waveYData[NodeMax];          //通道Y值储存
};

#endif // REALTIMEWAVE_H
