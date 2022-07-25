#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>
#include <QFileDialog>
#include <QThread>
#include <QPainter>
#include <QClipboard>
#include <QScrollEvent>
#include <QScrollBar>
#include <QTimerEvent>
#include <QTranslator>

#include <QtAlgorithms>
#include <algorithm>
#include <QProcess>

#include "advlog_parser.h"
#include "about_dialog.h"
#include "main_window.h"
#include "./ui_main_window.h"
#include "version.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _advLogReader(new AdvLogReader())
{
    connect(this, &MainWindow::AdvLogDoRead, _advLogReader, &AdvLogReader::doRead);
    connect(_advLogReader, &AdvLogReader::failedRead, this, &MainWindow::AdvLogFailedRead);
    connect(_advLogReader, &AdvLogReader::partialRead, this, &MainWindow::AdvLogPartialRead);
    connect(_advLogReader, &AdvLogReader::completedRead, this, &MainWindow::AdvLogCompletedRead);

    connect(this, &MainWindow::UpdateAdvLogInfo, this, &MainWindow::AdvLogInfoUpdated);

    qDebug() << _transEn.load(":/i18n/AdvlogBinViewer_en_US");
    qDebug() << _transZh.load(":/i18n/AdvlogBinViewer_zh_CN");

    ui->setupUi(this);

    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "AdvlogBinViewer_" + QLocale(locale).name();
        if (QLocale(locale).name() == "en_US") {
            qApp->removeTranslator(&_transZh);
            qApp->installTranslator(&_transEn);
            ui->actionEnglish->setChecked(true);
            ui->actionSimplified_Chinese->setChecked(false);
            break;
        } else if (QLocale(locale).name() == "zh_CN") {
            qApp->removeTranslator(&_transEn);
            qApp->installTranslator(&_transZh);
            ui->actionEnglish->setChecked(false);
            ui->actionSimplified_Chinese->setChecked(true);
            break;
        } else {

        }
    }

    ui->retranslateUi(this);

    ui->LbaTimeWidget->installEventFilter(this);
    ui->LbaTimeWidget->verticalScrollBar()->installEventFilter(this);

}

MainWindow::~MainWindow()
{
    delete _advLogReader;
    delete ui;
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui->LbaTimeWidget) {
        //qDebug() << event->type() << "\n";
    } else if (target == ui->LbaTimeWidget->verticalScrollBar()) {
        //qDebug() << event->type() << "\n";
        if (event->type() == QEvent::Paint) {
            //qDebug() << ui->LbaTimeWidget->verticalScrollBar()->value() << "\n";
            // update table
            if (_advLogInfoOnFilter) { // promise enabled
                emit UpdateAdvLogInfo(ui->LbaTimeWidget->verticalScrollBar()->value());
            }
        }
    }
    return QMainWindow::eventFilter(target, event);
}

void MainWindow::AdvLogFailedRead(QString path)
{
    QMessageBox mb(
        QMessageBox::Icon::Critical,
        tr("Error"),
        tr("Failed to open") + " " + path,
        QMessageBox::Button::Ok,
        this);

    mb.exec();
}

void MainWindow::AdvLogPartialRead(QString path, char *data, int size)
{
    qDebug() << "reading " << size << " bytes into @" << (void *)data << "\n";
    statusBar()->showMessage(tr("Reading from") + " " + path + " ...");
    _advLogData.push_back(QByteArrayView(data, size));
    delete[] data;  // clear heap
}

void MainWindow::AdvLogCompletedRead(QString path)
{
    qDebug() << "totally read " << _advLogData.size() << " bytes into combined buffer\n";

    _advLogInfoAll = _advLogParser.parse(_advLogData);
    _advLogInfoOnFilter = _advLogParser.parse(_advLogData);
    if (!_advLogInfoOnFilter) {
        statusBar()->showMessage(tr("Error at parsing ADVLOG format from") + " " + path);
    } else {
        statusBar()->showMessage(tr("Successfully read from") + " " + path);
        emit UpdateAdvLogInfo(0);
    }
}

void MainWindow::AdvLogInfoUpdated(int begin_row)
{
    qDebug() << "======[ADVLOG INFOMATION]======\n";
    qDebug() << "      signature: " << _advLogInfoOnFilter->signature << "\n";
    qDebug() << "            ver: " << _advLogInfoOnFilter->ver << "\n";
    qDebug() << "           mode: " << _advLogInfoOnFilter->mode << "\n";
    qDebug() << "         device: " << _advLogInfoOnFilter->device << "\n";
    qDebug() << "             fw: " << _advLogInfoOnFilter->fw << "\n";
    qDebug() << "             sn: " << _advLogInfoOnFilter->sn << "\n";
    qDebug() << "        sectors: " << _advLogInfoOnFilter->sectors << "\n";
    qDebug() << "    sector_size: " << _advLogInfoOnFilter->sector_size << "\n";
    qDebug() << "scan_block_size: " << _advLogInfoOnFilter->scan_block_size << "\n";
    qDebug() << " lba_time count: " << _advLogInfoOnFilter->entries.size() << "\n";

    QPainter paint(this);

    ui->line_path->setText(_path);

    ui->line_signature->setText(_advLogInfoOnFilter->signature);
    ui->line_ver->setText(_advLogInfoOnFilter->ver);
    ui->line_mode->setText(_advLogInfoOnFilter->mode);
    ui->line_device->setText(_advLogInfoOnFilter->device);
    ui->line_fw->setText(_advLogInfoOnFilter->fw);
    ui->line_sn->setText(_advLogInfoOnFilter->sn);
    ui->line_sectors->setText(_advLogInfoOnFilter->sectors);
    ui->line_sector_size->setText(_advLogInfoOnFilter->sector_size);
    ui->line_scan_block_size->setText(_advLogInfoOnFilter->scan_block_size);
    ui->line_lba_time_entries->setText(QString::asprintf("%lld", _advLogInfoOnFilter->entries.size()).toLocal8Bit());

    int partial_display_rows = 30;

    // begin preventing overflow
    if (_advLogInfoOnFilter->entries.size() < partial_display_rows) {
        partial_display_rows = _advLogInfoOnFilter->entries.size();
    }
    if (begin_row > _advLogInfoOnFilter->entries.size() - partial_display_rows) {
        begin_row = _advLogInfoOnFilter->entries.size() - partial_display_rows;
    }
    // end preventing overflow

    statusBar()->showMessage(QString(tr("Read LBA/Time entries from row %1 to lazy loaded %2 rows")).arg(begin_row).arg(partial_display_rows));

    QTableWidgetItem **lbas = new QTableWidgetItem*[partial_display_rows];
    QTableWidgetItem **times = new QTableWidgetItem*[partial_display_rows];
    for (size_t i = 0; i < partial_display_rows; ++i) {
        lbas[i] = new QTableWidgetItem;
        times[i] = new QTableWidgetItem;
        lbas[i]->setText(QString::asprintf("%llu", _advLogInfoOnFilter->entries[i + begin_row].address));
        times[i]->setText(QString::asprintf("%llu", _advLogInfoOnFilter->entries[i + begin_row].time_ms));
    }

    ui->LbaTimeWidget->setRowCount(_advLogInfoOnFilter->entries.size());
    for (size_t i = 0; i < partial_display_rows; ++i) {
        ui->LbaTimeWidget->setItem(i + begin_row, 0, lbas[i]);
        ui->LbaTimeWidget->setItem(i + begin_row, 1, times[i]);
    }

    delete[] lbas;
    delete[] times;
}


void MainWindow::on_actionVisit_me_GitHub_V_triggered()
{
    QDesktopServices::openUrl(QUrl(APP_AUTHOR_URL));

}


void MainWindow::on_actionProject_Homepage_H_triggered()
{
    QDesktopServices::openUrl(QUrl(APP_HOMEPAGE_URL));
}


void MainWindow::on_actionAbout_A_triggered()
{
    AboutDialog dlg((QString(APP_REL_VERSION)));
    dlg.setWindowIcon(QIcon(":/images/icon.ico"));
    dlg.exec();
}


void MainWindow::on_actionOpen_O_triggered()
{
    QStatusBar * status_bar = statusBar();

    status_bar->showMessage(tr("Waiting open dialog..."));

    _path = QFileDialog::getOpenFileName(this,
        tr("Open binary file which MAKELOG command in MHDD.EXE that generate"),
        ".",
        tr("ADVLOG.BIN files (ADVLOG.BIN);;BIN files (*.BIN);;All files (*.*)"));

    if (_path.isEmpty()) {
        status_bar->showMessage(tr("Operation canceled"));
        return;
    }

    status_bar->showMessage(tr("Opening") + " " + _path + " ...");

    _advLogData.clear();
    emit AdvLogDoRead(_path);
}


void MainWindow::on_actionExit_Program_E_triggered()
{
    this->close();
}


void MainWindow::on_openButton_clicked()
{
    on_actionOpen_O_triggered();
}


void MainWindow::on_copyAllTextButton_clicked()
{
    QByteArray s;
    s += "  BIN file path: "; s += _path.toLocal8Bit(); s += "\n";
    s += "      signature: "; s += _advLogInfoOnFilter->signature.toLocal8Bit(); s += "\n";
    s += "            ver: "; s += _advLogInfoOnFilter->ver; s += "\n";
    s += "           mode: "; s += _advLogInfoOnFilter->mode; s += "\n";
    s += "         device: "; s += _advLogInfoOnFilter->device; s += "\n";
    s += "             fw: "; s += _advLogInfoOnFilter->fw; s += "\n";
    s += "             sn: "; s += _advLogInfoOnFilter->sn; s += "\n";
    s += "        sectors: "; s += _advLogInfoOnFilter->sectors; s += "\n";
    s += "    sector_size: "; s += _advLogInfoOnFilter->sector_size; s += "\n";
    s += "scan_block_size: "; s += _advLogInfoOnFilter->scan_block_size; s += "\n";
    s += " lba_time count: "; s += QString::asprintf("%llu", _advLogInfoOnFilter->entries.size()).toLocal8Bit(); s += "\n";

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(s);
}


void MainWindow::on_sortLbaButton_clicked()
{
    if (!_advLogInfoOnFilter)
        return;

    static bool flag = false;
    std::sort(_advLogInfoOnFilter->entries.begin(), _advLogInfoOnFilter->entries.end(),
    [](LBA_Time & a, LBA_Time & b) -> bool {
        return flag ? a.address < b.address : a.address > b.address;
    });
    flag = !flag;
    emit UpdateAdvLogInfo(0);
    ui->LbaTimeWidget->verticalScrollBar()->setValue(ui->LbaTimeWidget->verticalScrollBar()->value());
}


void MainWindow::on_sortTimeButton_clicked()
{
    if (!_advLogInfoOnFilter)
        return;

    static bool flag = false;
    std::sort(_advLogInfoOnFilter->entries.begin(), _advLogInfoOnFilter->entries.end(),
    [](LBA_Time & a, LBA_Time & b) -> bool {
        return flag ? a.time_ms < b.time_ms : a.time_ms > b.time_ms;
    });
    flag = !flag;
    emit UpdateAdvLogInfo(0);
    ui->LbaTimeWidget->verticalScrollBar()->setValue(ui->LbaTimeWidget->verticalScrollBar()->value());
}


void MainWindow::on_applyFilterButton_clicked()
{
    if (!_advLogInfoOnFilter)
        return;

    _advLogInfoOnFilter.reset(new AdvLogInfoRaw(*_advLogInfoAll));
    if (ui->radioButton_All->isChecked()) {

    } else if (ui->radioButton_Between->isChecked()) {
        _advLogInfoOnFilter->entries.removeIf([&](const LBA_Time & e) -> bool {
            return e.time_ms < ui->spinBox_betweenfrom->value()
                || e.time_ms > ui->spinBox_betweento->value();
        });

    } else if (ui->radioButton_Morethan->isChecked()) {
        _advLogInfoOnFilter->entries.removeIf([&](const LBA_Time & e) -> bool {
            return e.time_ms < ui->spinBox_morethan->value();
        });

    } else if (ui->radioButton_Lessthan->isChecked()) {

        _advLogInfoOnFilter->entries.removeIf([&](const LBA_Time & e) -> bool {
            return e.time_ms > ui->spinBox_lessthan->value();
        });
    }

    emit UpdateAdvLogInfo(0);
}


void MainWindow::on_actionClose_C_triggered()
{
    qApp->quit();
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
}


void MainWindow::on_actionMeta_Info_M_triggered()
{
    ui->tabWidget->setCurrentIndex(0);
}


void MainWindow::on_actionREsults_List_L_triggered()
{
    ui->tabWidget->setCurrentIndex(1);
}


void MainWindow::on_resetDefaultFilterButton_clicked()
{
    ui->radioButton_All->setChecked(true);
    on_applyFilterButton_clicked();
}


void MainWindow::on_actionEnglish_triggered()
{
    ui->actionEnglish->setChecked(true);
    ui->actionSimplified_Chinese->setChecked(false);

    qApp->removeTranslator(&_transZh);
    qApp->installTranslator(&_transEn);
    ui->retranslateUi(this);
}


void MainWindow::on_actionSimplified_Chinese_triggered()
{
    ui->actionEnglish->setChecked(false);
    ui->actionSimplified_Chinese->setChecked(true);

    qApp->removeTranslator(&_transEn);
    qApp->installTranslator(&_transZh);
    ui->retranslateUi(this);
}

