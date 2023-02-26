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

    if (QString::compare(format, "MPEG Audio") == 0) {
        auto profile = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, 0, __T("Format_Profile")));
        if (QString::compare(profile, "Layer 3") == 0) format = "MP3";
        else if (QString::compare(profile, "Layer 2") == 0) format = "MP2";
    }

    //cout << QString::fromStdWString(MI.Inform()).toStdString();

    values << mfile.absolutePath()
           << mfile.fileName()
           << convertByte((float) mfile.size())
           << setFormat(format)
           << convertBit(statistics.bit_rate)
           << statistics.duration.toString(QString::fromLatin1("HH:mm:ss"));

    /** Informações de vídeo */
    auto w = statistics.video_only.width;
    auto h = statistics.video_only.height;
    auto videoformat = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("Format")));
    auto videofmtinfo = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("Format/Info")));
    auto videocodec = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("CodecID")));
    auto videocinfo = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("CodecID/Info")));
    auto bitdepth = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("BitDepth")));

    if (videofmtinfo.isEmpty()) valuesVideo << setFormatInfo(videoformat);
    else valuesVideo << QString::fromLatin1("%1 (%2)").arg(videoformat, videofmtinfo);

    if (videocinfo.isEmpty()) valuesVideo << setCodecInfo(videocodec);
    else valuesVideo << QString::fromLatin1("%1 (%2)").arg(videocodec, videocinfo);

    valuesVideo << convertBit(statistics.video.bit_rate)
                << convertAspectRatio(w, h);

    if (w == 0 || h == 0) valuesVideo << "";
    else valuesVideo << QString::fromLatin1("%1x%2").arg(w).arg(h);

    if (statistics.video.frame_rate == 0) valuesVideo << "";
    else valuesVideo << QString::number(statistics.video.frame_rate, 'f', 2);

    if (statistics.video.frames == 0) valuesVideo << "";
    else valuesVideo << QString::number(statistics.video.frames);

    if (bitdepth.isEmpty()) valuesVideo << "";
    else valuesVideo << bitdepth + " bits";

    valuesVideo << QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("ChromaSubsampling")));

    int i = 0;
    /** Informações de áudio com suporte dual áudio */
    foreach(QStringList audio, listAudio) {
        QtAV::Statistics actual{};
        if (i == 0) actual = statistics;
        else if (i == 1) actual = dualstatistics;

        auto channel = actual.audio_only.channels;
        auto channellayout = actual.audio_only.channel_layout;
        auto audioformat = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, i, __T("Format")));
        auto audiofmtinfo = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, i, __T("Format/Info")));
        auto audiocodec = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, i, __T("CodecID")));
        auto audiobitdepth = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, i, __T("BitDepth")));

        if (audiofmtinfo.isEmpty()) audio << setFormatInfo(audioformat);
        else audio << QString::fromLatin1("%1 (%2)").arg(audioformat, audiofmtinfo);

        audio << setCodecInfo(audiocodec)
              << convertBit(actual.audio.bit_rate)
              << convertHz(actual.audio_only.sample_rate)
              << actual.audio_only.sample_fmt;

        if (channel == 0) audio << "";
        else if (channellayout.isEmpty()) audio << QString::number(channel);
        else audio << QString::fromLatin1("%1 (layout %2)").arg(channel).arg(channellayout);

        if (actual.audio.frame_rate == 0) audio << "";
        else audio << QString::number(actual.audio.frame_rate);

        if (actual.audio_only.frame_size == 0) audio << "";
        else audio << QString::number(actual.audio_only.frame_size);

        if (actual.audio.frames == 0) audio << "";
        else audio << QString::number(actual.audio.frames);

        if (audiobitdepth.isEmpty()) audio << "";
        else audio << audiobitdepth + " bits";

        /** Gravando as informações */
        if (i == 0) valuesAudio << audio;
        else if (i == 1) valuesDual << audio;
        i++;
    }

    if (!statistics.metadata.isEmpty()) {
        QStringList keys{statistics.metadata.keys()};
        for (const QString &inf: infos) {
            if (keys.contains(inf, Qt::CaseInsensitive)) {
                if (inf == "date") {
                    QDate dt = QDate::fromString(statistics.metadata.value(
                            keys.filter(inf, Qt::CaseInsensitive).at(0)), "yyyyMMdd");
                    metadata << dt.toString("yyyy-MM-dd");
                } else if (inf == "track") {
                    QStringList tm = keys.filter("track", Qt::CaseInsensitive);
                    if (tm.contains("tracktotal", Qt::CaseInsensitive)) {

                        QString tot = statistics.metadata.value(tm.filter("tracktotal", Qt::CaseInsensitive).at(0));
                        tm.removeOne(keys.filter("tracktotal", Qt::CaseInsensitive).at(0));
                        QString tr = statistics.metadata.value(tm.filter("track", Qt::CaseInsensitive).at(0));

                        if (QString::compare(tot, "") == 0)
                            metadata << statistics.metadata.value(keys.filter(inf, Qt::CaseInsensitive).at(0));
                        else metadata << QString::fromLatin1("%1 / %2").arg(tr, tot);

                    } else metadata << statistics.metadata.value(keys.filter(inf, Qt::CaseInsensitive).at(0));
                } else metadata << statistics.metadata.value(keys.filter(inf, Qt::CaseInsensitive).at(0));
            } else metadata << "";
        }
    }

    Q_EMIT baseValues(values, valuesVideo, valuesAudio, valuesDual, metadata, format);
    MI.Close();

    DG_T << "Finalizando o thread " << QThread::currentThreadId();
    Q_EMIT finished();
}


/** Reescrevendo o texto de alguns formatos */
QString StatisticsWorker::setFormat(const QString &format) const {
    switch (Hash::hash(format.toStdString())) {
        case "MPEG-4"_hash:   return "mp4 - MPEG-4 (Moving Picture Expert Group 4)";
        case "Matroska"_hash: return "mkv - Matroska (Matroska Video)";
        case "WebM"_hash:     return "webm - WebM (Web Media)";
        case "MP3"_hash:      return "mp3 - MP3 (MPEG Audio Layer 3)";
        case "MP2"_hash:      return "mp2 - MP2 (MPEG Audio Layer 2)";
        default:              return statistics.format;
    }
}


/** Reescrevendo algumas informações de formatos de vídeo e áudio */
QString StatisticsWorker::setFormatInfo(const QString &finfo) {
    switch (Hash::hash(finfo.toStdString())) {
        case "PCM"_hash:  return QString::fromLatin1("%1 (%2)").arg(finfo, "Pulse-Code Modulation");
        case "DV"_hash:   return QString::fromLatin1("%1 (%2)").arg(finfo, "Digital Video Encoding");
        case "FFV1"_hash: return QString::fromLatin1("%1 (%2)").arg(finfo, "FF Video Codec 1");
        case "YUV"_hash:  return QString::fromLatin1("%1 (%2)").arg(finfo, "YUV Format");
        case "VP9"_hash:  return QString::fromLatin1("%1 (%2)").arg(finfo, "VP9 Video Format");
        case "VP8"_hash:  return QString::fromLatin1("%1 (%2)").arg(finfo, "VP8 Video Format");
        case "Opus"_hash: return QString::fromLatin1("%1 (%2)").arg(finfo, "Opus Interactive Audio Format");
        default: return finfo;
    }
}


/** Reescrevendo informações de alguns codecs */
QString StatisticsWorker::setCodecInfo(const QString &cinfo) {
    switch (Hash::hash(cinfo.toStdString())) {
        case "V_VP9"_hash:  return QString::fromLatin1("%1 (%2)").arg(cinfo, "VP9 Codec Format");
        case "V_VP8"_hash:  return QString::fromLatin1("%1 (%2)").arg(cinfo, "VP8 Codec Format");
        case "A_OPUS"_hash: return QString::fromLatin1("%1 (%2)").arg(cinfo, "Opus Interactive Audio Codec");
        case "A_EAC3"_hash: return QString::fromLatin1("%1 (%2)").arg(cinfo, "Enhanced AC-3 Codec");
        case "v210"_hash:   return QString::fromLatin1("%1 (%2)").arg(cinfo, "V210 Video Codec");
        default: return cinfo;
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


/** Setando as informações a serem processadas */
void StatisticsWorker::setFile(const QString &val, const QtAV::Statistics &s, const QtAV::Statistics &d) {
    file = val;
    statistics = s;
    dualstatistics = d;
}

#undef DG_T
