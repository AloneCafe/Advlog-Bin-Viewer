#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QObject>
#include <QString>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class AboutDialog;
}
QT_END_NAMESPACE

class AboutDialog : public QDialog
{
    Q_OBJECT
public:
    AboutDialog(QString version, QWidget *parent = nullptr);
    ~AboutDialog();

private slots:

    void on_pushButton_clicked();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
