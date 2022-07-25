#include "advlog_parser.h"

#include <QFile>
#include <QThread>

AdvLogParser::AdvLogParser(QObject *parent)
    : QObject{parent}
{

}

QSharedPointer<AdvLogInfoRaw> AdvLogParser::parse(const QByteArray &raw)
{
    QSharedPointer<AdvLogInfoRaw> info(new AdvLogInfoRaw);

    if (raw.size() < sizeof(AdvLogInfoTpl::signature)) {
        return nullptr;
    }

    QLatin1String *easy_arr[9] = {
        nullptr,
        &info->ver,
        &info->mode,
        &info->device,
        &info->fw,
        &info->sn,
        &info->sectors,
        &info->sector_size,
        &info->scan_block_size,
    };
    size_t idx = 0;

    const char * p;
    for (p = raw.constData(); p < raw.end(); ) {
        for (const char * ep = p; ep < raw.end() - 1; ++ep) {
            if (*ep == 0xD && *(ep + 1) == 0xA) {
                if (idx == 0) {
                    QByteArray s;
                    const char * cp;
                    for (cp = p; cp < ep - 1; ++cp) {
                        s += byte2HexStrUppercase(*cp);
                        s += ' ';
                    }
                    s += byte2HexStrUppercase(*(ep - 1));
                    info->signature = QString(s);
                    idx++;
                } else if (idx < 8) {
                    *easy_arr[idx++] = QLatin1String(p, ep);
                } else { // idx == 8
                    *easy_arr[idx++] = QLatin1String(p, ep);
                    p = ep + 2;
                    goto read_entries;
                }
                p = ep + 2;
                goto next_field;
            }
        }
        p += 1;
    next_field:
        continue;
    }
    read_entries:

    if (idx != sizeof(easy_arr) / sizeof(*easy_arr)) { // incomplete meta info
        return nullptr;
    }

    for (idx = 0; p <= raw.end() - sizeof(LBA_Time); ++idx) {
        LBA_Time ent;
        ent.address = *reinterpret_cast<const uint64_t *>(p);
        ent.time_ms = *reinterpret_cast<const uint64_t *>(p + sizeof(uint64_t));

        info->entries.push_back(ent);
        p += sizeof(LBA_Time);
    }

    return info;
}

QByteArray AdvLogParser::byte2HexStrUppercase(unsigned char b)
{
    QByteArray arr;
    arr.push_back(hexDigit2HexCharUppercase(b / 0x10));
    arr.push_back(hexDigit2HexCharUppercase(b % 0x10));
    return arr;
}

char AdvLogParser::hexDigit2HexCharUppercase(unsigned char digit)
{
    return digit < 10 ? '0' + digit : ('A' + digit - 10);
}
