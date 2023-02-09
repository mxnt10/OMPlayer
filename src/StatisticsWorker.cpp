#include <QFileInfo>
#include <QThread>
#include <Utils>

#include "Hash.hpp"
#include "StatisticsWorker.h"

using namespace Hash::literals;

#define DG_T qDebug().nospace() << GRE << "(" << RED << "Thread" << GRE << ")" << RED << "::" << RDL


/**********************************************************************************************************************/


/** Construtor
 *
 * QThread::HighestPriority
 * QThread::LowestPriority
 *
 * */
StatisticsWorker::StatisticsWorker(QObject *parent) : QObject(parent) {}


/**********************************************************************************************************************/


/** Thread para obter o hash md5 de um arquivo */
void StatisticsWorker::doWork() {
    DG_T << "Iniciando o thread " << QThread::currentThreadId();
    QStringList infos{"title", "artist", "album", "genre", "track", "date", "purl"};
    QStringList values, valuesVideo, valuesAudio, valuesDual, metadata;
    QList<QStringList> listAudio{valuesAudio, valuesDual};

    QFileInfo mfile{file};
    MI.Open(file.toStdWString());

    auto format = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_General, 0, __T("Format")));
    auto duration = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_General, 0, __T("Duration")));
    auto bitrate = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_General, 0, __T("OverallBitRate")));

    if (QString::compare(format, "MPEG Audio") == 0) {
        auto profile = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, 0, __T("Format_Profile")));
        if (QString::compare(profile, "Layer 3") == 0) format = "MP3";
        else if (QString::compare(profile, "Layer 2") == 0) format = "MP2";
    }

    qDebug() << format << statistics.format;
    cout << QString::fromStdWString(MI.Inform()).toStdString();

    values << QString(file).remove(QRegExp("\\/(?:.(?!\\/))+$"))
           << QString(file).remove(QRegExp("\\/.+\\/"))
           << convertByte((float) mfile.size())
           << setFormat(format)
           << convertBit(bitrate.toFloat())
           << QTime(0, 0, 0).addMSecs(duration.toInt()).toString(QString::fromLatin1("HH:mm:ss"));

    /** Informações de vídeo */
    auto w = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("Width")));
    auto h = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("Height")));
    auto videoformat = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("Format")));
    auto videofmtinfo = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("Format/Info")));
    auto videocodec = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("CodecID")));
    auto videocinfo = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("CodecID/Info")));
    auto videobitrate = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("BitRate")));
    auto framerate = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("FrameRate")));
    auto bitdepth = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("BitDepth")));

    if (videofmtinfo.isEmpty()) valuesVideo << videoformat;
    else valuesVideo << QString::fromLatin1("%1 (%2)").arg(videoformat, videofmtinfo);

    if (videocinfo.isEmpty()) valuesVideo << videocodec;
    else valuesVideo << QString::fromLatin1("%1 (%2)").arg(videocodec, videocinfo);

    valuesVideo << convertBit(videobitrate.toFloat())
                << convertAspectRatio(w.toInt(), h.toInt());

    if (w.toInt() == 0 || h.toInt() == 0) valuesVideo << "";
    else valuesVideo << QString::fromLatin1("%1x%2").arg(w, h);

    if (framerate.toFloat() == 0) valuesVideo << "";
    else valuesVideo << QString::number(framerate.toFloat(), 'f', 2);

    if (bitdepth.isEmpty()) valuesVideo << "";
    else valuesVideo << bitdepth + " bits";

    valuesVideo << QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("ChromaSubsampling")));

    int i = 0;
    /** Informações de áudio com suporte dual áudio */
    foreach(QStringList audio, listAudio) {
        auto audioformat = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, i, __T("Format")));
        auto audiofmtinfo = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, i, __T("Format/Info")));
        auto audiocodec = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, i, __T("CodecID")));
        auto audiobitrate = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, i, __T("BitRate")));
        auto samplerate = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, i, __T("SamplingRate")));
        auto channel = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, i, __T("Channels")));
        auto channellayout = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, i, __T("ChannelLayout")));
        auto audioframerate = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, i, __T("FrameRate")));
        auto audiobitdepth = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, i, __T("BitDepth")));

        if (QString::compare(channellayout, "L R") == 0) channellayout = "stereo";
        else if (QString::compare(channellayout, "C") == 0) channellayout = "mono";
        else if (channellayout.isEmpty()) {
            if (channel.toInt() == 2) channellayout = "stereo";
            else if (channel.toInt() == 1) channellayout = "mono";
        }

        if (audiofmtinfo.isEmpty()) audio << audioformat;
        else audio << QString::fromLatin1("%1 (%2)").arg(audioformat, audiofmtinfo);

        audio << audiocodec;

        if (audiobitrate.isEmpty()) audio << "";
        else audio << convertBit(audiobitrate.toFloat());

        audio << convertHz(samplerate.toFloat());

        if (channel.isEmpty()) audio << "";
        else if (channellayout.isEmpty()) audio << channel;
        else audio << QString::fromLatin1("%1 (layout %2)").arg(channel, channellayout);

        if (audioframerate.isEmpty()) audio << "";
        else audio << QString::number(audioframerate.toFloat(), 'f', 2);

        if (audiobitdepth.isEmpty()) audio << "";
        else audio << audiobitdepth + " bits";
        listAudio[i] << audio;
        i++;
    }
    valuesAudio << listAudio[0];
    valuesDual << listAudio[1];

    if (!statistics.metadata.isEmpty()) {
        QStringList keys{statistics.metadata.keys()};
        for (const QString &inf: infos) {
            if (keys.contains(inf, Qt::CaseInsensitive)) {
                if (inf == "date") {
                    QDate dt = QDate::fromString(statistics.metadata.value(
                            keys.filter(inf, Qt::CaseInsensitive)[0]), "yyyyMMdd");
                    metadata << dt.toString("yyyy-MM-dd");
                } else if (inf == "track") {
                    QStringList tm = keys.filter("track", Qt::CaseInsensitive);
                    if (tm.contains("tracktotal", Qt::CaseInsensitive)) {

                        QString tot = statistics.metadata.value(tm.filter("tracktotal", Qt::CaseInsensitive)[0]);
                        tm.removeOne(keys.filter("tracktotal", Qt::CaseInsensitive)[0]);
                        QString tr = statistics.metadata.value(tm.filter("track", Qt::CaseInsensitive)[0]);

                        if (tot == "") metadata << statistics.metadata.value(keys.filter(inf, Qt::CaseInsensitive)[0]);
                        else metadata << QString::fromLatin1("%1 / %2").arg(tr, tot);

                    } else metadata << statistics.metadata.value(keys.filter(inf, Qt::CaseInsensitive)[0]);
                } else metadata << statistics.metadata.value(keys.filter(inf, Qt::CaseInsensitive)[0]);
            } else metadata << "";
        }
    }

    Q_EMIT baseValues(values, valuesVideo, valuesAudio, valuesDual, metadata, format,
                      duration.toInt(), {rat, w, h});
    MI.Close();

    DG_T << "Finalizando o thread " << QThread::currentThreadId();
    Q_EMIT finished();
}


/** Reescrevendo o texto de alguns formatos */
QString StatisticsWorker::setFormat(const QString &format) {
    switch (Hash::hash(format.toStdString())) {
        case "MPEG-PS"_hash:  return "mpeg - MPEG-PS (MPEG-2 Program Stream)";
        case "MPEG-4"_hash:   return "mp4 - MPEG-4 (Moving Picture Expert Group 4)";
        case "Matroska"_hash: return "mkv - Matroska (Matroska Video)";
        case "WebM"_hash:     return "webm - WebM (Web Media)";
        case "DV"_hash:       return "dv - DV (Digital Video)";
        case "MP3"_hash:      return "mp3 - MP3 (MPEG Audio Layer 3)";
        case "MP2"_hash:      return "mp2 - MP2 (MPEG Audio Layer 2)";
        case "Wave"_hash:     return "wav - WAV / WAVE (Waveform Audio)";
        default:              return format;
    }
}


/** Definindo o tamanho do arquivo de mídia */
QString StatisticsWorker::convertByte(auto byte) {
    if (byte == 0) return {""};

    int i = 0;
    auto size = byte;
    QString bytes{"B"};
    QStringList byteit{"KiB", "MiB", "GiB"};

    while (size > 1024) {
        size = size / 1024;
        bytes = byteit[i];
        i++;
    }

    /** Usando definição de variável "auto", precisa especificar o tipo no retorno */
    return QString::fromLatin1("%1 %2 (%3)").arg(QString::number(size, 'f', 2), bytes).arg((int)byte);
}


/** Definindo a taxa de bits */
QString StatisticsWorker::convertBit(auto bit) {
    if (bit == 0) return {""};

    int i = 0;
    QString bits{"b/s"};
    QStringList bitit{"Kb/s", "Mb/s", "Gb/s"};

    auto bitrate = bit;
    while (bitrate > 1000) {
        bitrate = bitrate / 1000;
        bits = bitit[i];
        i++;
    }

    return QString::fromLatin1("%1 %2").arg(QString::number(bitrate, 'f', 2), bits);
}


/** Definindo as opções de aspect ratio */
QString StatisticsWorker::convertAspectRatio(int x, int y) {
    if (x == 0 || y == 0) return {""};

    int w = x;
    int h = y;
    int dividend, divisor;

    if (h == w) {
        return "1:1";
    } else {
        QString mode{};
        if (h > w) {
            dividend = h;
            divisor = w;
            mode = "Portrait";
        } else if (w > h) {
            dividend = w;
            divisor = h;
            mode = "Landscape";
        }

        QString ratio{mode + " " + QString::number(((float)dividend / (float)divisor))};

        uint gcd = -1;
        int remainder;
        while (gcd == -1) {
            remainder = dividend % divisor;
            if (remainder == 0) {
                gcd = divisor;
            } else {
                dividend = divisor;
                divisor = remainder;
            }
        }

        QString hr = QString::number(w / gcd);
        QString vr = QString::number(h / gcd);
        rat = hr + ":" + vr;
        return QString::fromLatin1("%1 (%2)").arg(hr + ":" + vr, ratio);
    }
}


/** Definição da frequência */
QString StatisticsWorker::convertHz(auto hz) {
    if (hz == 0) return {""};

    int i = 0;
    QString herts{"Hz"};
    QStringList hertsit{"kHz", "mHz", "gHz"};

    auto hert = hz;
    while (hert > 1000) {
        hert = hert / 1000;
        herts = hertsit[i];
        i++;
    }

    return QString::number(hert) + " " + herts;
}

#undef DG_T
