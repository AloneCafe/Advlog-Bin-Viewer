#include "main_window.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QIcon>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(style);
    QApplication a(argc, argv);


    MainWindow w;
    w.setWindowIcon(QIcon(":/images/icon.ico"));
    w.show();
    return a.exec();
}
