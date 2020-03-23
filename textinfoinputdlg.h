#ifndef TEXTINFOINPUTDLG_H
#define TEXTINFOINPUTDLG_H

#include <QDialog>

namespace Ui {
class TextInfoInputDlg;
}

class TextInfoInputDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TextInfoInputDlg(QWidget *parent = nullptr);
    ~TextInfoInputDlg();

signals:
    void text(const QString str);

private slots:

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::TextInfoInputDlg *ui;
};

#endif // TEXTINFOINPUTDLG_H
