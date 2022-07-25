#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "advlog_parser.h"
#include <QMainWindow>
#include <QThread>
#include <QTranslator>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    bool eventFilter(QObject * target, QEvent *event) override;

signals:
    void AdvLogDoRead(QString path);
    void UpdateAdvLogInfo(int begin_row);

public slots:
    void AdvLogFailedRead(QString path);
    void AdvLogPartialRead(QString path, char *data, int size);
    void AdvLogCompletedRead(QString path);
    void AdvLogInfoUpdated(int begin_row);

private slots:
    void on_actionVisit_me_GitHub_V_triggered();

    void on_actionProject_Homepage_H_triggered();

    void on_actionAbout_A_triggered();

    void on_actionOpen_O_triggered();

    void on_actionExit_Program_E_triggered();

    void on_openButton_clicked();

    void on_copyAllTextButton_clicked();

    void on_sortLbaButton_clicked();

    void on_sortTimeButton_clicked();

    void on_applyFilterButton_clicked();

    void on_actionClose_C_triggered();

    void on_actionMeta_Info_M_triggered();

    void on_actionREsults_List_L_triggered();

    void on_resetDefaultFilterButton_clicked();

    void on_actionEnglish_triggered();

    void on_actionSimplified_Chinese_triggered();

private:
    AdvLogReader *_advLogReader;
    QThread _threadReader;

    QString _path;
    AdvLogParser _advLogParser;
    QByteArray _advLogData;
    QSharedPointer<AdvLogInfoRaw> _advLogInfoOnFilter, _advLogInfoAll;

    Ui::MainWindow *ui;
    QTranslator _transZh, _transEn;
};
#endif // MAINWINDOW_H
