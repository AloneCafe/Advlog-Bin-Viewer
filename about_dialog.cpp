#include <QDialog>
#include <QObject>
#include <QWidget>

#include "about_dialog.h"
#include "./ui_about_dialog.h"

AboutDialog::AboutDialog(QString version, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->edit_version->setText(version);
}


AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_pushButton_clicked()
{
    this->close();
}

