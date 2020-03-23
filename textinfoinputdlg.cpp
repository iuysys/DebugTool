#include "textinfoinputdlg.h"
#include "ui_textinfoinputdlg.h"

TextInfoInputDlg::TextInfoInputDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextInfoInputDlg)
{
    ui->setupUi(this);
}

TextInfoInputDlg::~TextInfoInputDlg()
{
    delete ui;
}

void TextInfoInputDlg::on_buttonBox_accepted()
{
    QString str = ui->lineEdit->text();
    ui->lineEdit->clear();

    if(!str.isEmpty()){
        emit text(str);
    }
    this->hide();
}

void TextInfoInputDlg::on_buttonBox_rejected()
{
    ui->lineEdit->clear();
    this->hide();
}
