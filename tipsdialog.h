#ifndef TIPSDIALOG_H
#define TIPSDIALOG_H

#include <QDialog>
#include <QTimer>

class TipsDialog : public QDialog
{
    Q_OBJECT
public:
    TipsDialog(QWidget *parent = nullptr);


private:
    QTimer *mTimer;
};

#endif // TIPSDIALOG_H
