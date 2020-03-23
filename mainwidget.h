#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>


namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();


private:
    Ui::MainWidget *ui;
    int mCurrentWidgetIndex;

    //System Setting


protected:
    void closeEvent(QCloseEvent *event);


private slots:
    void MessageProcess(void);
    void on_tabWidget_currentChanged(int index);

};

#endif // MAINWIDGET_H
