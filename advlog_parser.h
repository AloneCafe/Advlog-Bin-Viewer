#ifndef ADVLOGPARSER_H
#define ADVLOGPARSER_H

#include "qdebug.h"
#include <QFile>
#include <QObject>
#include <QSharedPointer>
#include <QString>

#pragma pack(push, 1)
struct LBA_Time {
    uint64_t address;
    uint64_t time_ms;
};

struct AdvLogInfoTpl {
    uint32_t signature; // 9E 00 00 00
    uint16_t __0x0d0a_0;  // 0D 0A

    QLatin1String  line_ver;
    uint16_t __0x0d0a_1;  // 0D 0A

    QLatin1String  line_mode;
    uint16_t __0x0d0a_2;  // 0D 0A

    QLatin1String  line_device;
    uint16_t __0x0d0a_3;  // 0D 0A

    QLatin1String  line_fw;
    uint16_t __0x0d0a_4;  // 0D 0A

    QLatin1String  line_sn;
    uint16_t __0x0d0a_5;  // 0D 0A

    QLatin1String  line_sectors;
    uint16_t __0x0d0a_6;  // 0D 0A

    QLatin1String  line_sector_size;
    uint16_t __0x0d0a_7;  // 0D 0A

    QLatin1String  line_scan_block_size;
    uint16_t __0x0d0a_8;  // 0D 0A

    QList<LBA_Time> entries;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct AdvLogInfoRaw {
          QString  signature;
    QLatin1String  ver;
    QLatin1String  mode;
    QLatin1String  device;
    QLatin1String  fw;
    QLatin1String  sn;
    QLatin1String  sectors;
    QLatin1String  sector_size;
    QLatin1String  scan_block_size;

    QList<LBA_Time> entries;
};
#pragma pack(pop)

class AdvLogReader : public QObject {
    Q_OBJECT

public slots:
    void doRead(QString path) {
        qDebug() << "starting reading" << "\n";
        // block operation
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly)) {
            emit failedRead(path);
            return;
        }

        QDataStream in(&f);
        constexpr int bytes_per_read = 1024 * 1024; // 1MB
        while (!in.atEnd()) {
            char *data = new char[bytes_per_read];
            int size = in.readRawData(data, bytes_per_read);
            if (size < 0) {
                emit failedRead(path);
                return;
            }
            emit partialRead(path, data, size);
        }

        emit completedRead(path);
    }

signals:
    void failedRead(QString path);
    void partialRead(QString path, char *data, int size);
    void completedRead(QString path);
};




class AdvLogParser : public QObject
{
    Q_OBJECT
public:
    explicit AdvLogParser(QObject *parent = nullptr);

    QSharedPointer<AdvLogInfoRaw> parse(const QByteArray &);

private:
    QByteArray byte2HexStrUppercase(unsigned char b);
    char hexDigit2HexCharUppercase(unsigned char digit);

signals:

};

#endif // ADVLOGPARSER_H
