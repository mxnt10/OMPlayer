#include <QThread>
#include <Utils>
#include <iostream>
#include <QFileInfo>

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


/** Função para chamar o thread */
void StatisticsWorker::requestWork() {
    DG_T << "Solicitando início do thread " << QThread::currentThreadId();
    Q_EMIT workRequested();
}


/** Thread para obter o hash md5 de um arquivo */
void StatisticsWorker::doWork() {
    DG_T << "Iniciando o thread " << QThread::currentThreadId();
    QStringList values, valuesVideo, valuesAudio, valuesDual;
    QFileInfo mfile{file};
    MI.Open(file.toStdWString());

    values << QString(file).remove(QRegExp("\\/(?:.(?!\\/))+$"))
           << QString(file).remove(QRegExp("\\/.+\\/"))
           << convertByte((float)mfile.size());

    if (statistics.url.isEmpty()) {
        /** Usando MediaInfoDLL para buscar as informações */
        int duration = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_General, 0, __T("Duration"))).toInt();
        auto bitrate = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_General, 0, __T("OverallBitRate")));
        int w = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("Width"))).toInt();
        int h = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("Height"))).toInt();
        auto videobitrate = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("BitRate")));

        values << setFormat(QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_General, 0, __T("Format"))))
               << convertBit(bitrate.toFloat())
               << QTime(0, 0, 0).addMSecs(duration).toString(QString::fromLatin1("HH:mm:ss"));

        /** Informações de vídeo */
        valuesVideo << "" << ""; // Codec e Decoder não informados

        if (videobitrate.isEmpty()) valuesVideo << "";
        else valuesVideo << convertBit(videobitrate.toFloat());

        valuesVideo << convertAspectRatio(w, h)
                    << QString::fromLatin1("%1x%2").arg(w).arg(h);

    } else {
        int w = statistics.video_only.width;
        int h = statistics.video_only.height;
        int cw = statistics.video_only.coded_width;
        int ch = statistics.video_only.coded_height;
        auto sizev = QString::fromLatin1("%1x%2").arg(w).arg(h);
        auto sizec = QString::fromLatin1("%1x%2").arg(cw).arg(ch);

        /** Informações básicas */
        if (QString::compare(statistics.format.split(' ')[0], "mp3") == 0) values << "mp3 - MP3 (MPEG Audio Layer 3)";
        else if (QString::compare(statistics.format.split(' ')[2], "QuickTime") == 0 ||
                 QString::compare(statistics.format.split(' ')[2], "Matroska" ) == 0 ) {
            values << setFormat(QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_General, 0, __T("Format"))));
        } else values << statistics.format;

        values << convertBit(statistics.bit_rate)
               << "00:00:00 / " + statistics.duration.toString(QString::fromLatin1("HH:mm:ss"));

        /** Informações de vídeo */
        valuesVideo << QString::fromLatin1("%1 (%2)").arg(statistics.video.codec, statistics.video.codec_long)
                    << QString::fromLatin1("%1 (%2)").arg(statistics.video.decoder, statistics.video.decoder_detail);

        if (statistics.video.bit_rate == 0) valuesVideo << "";
        else valuesVideo << convertBit((float)statistics.video.bit_rate);

        valuesVideo << convertAspectRatio(w, h)
                    << QString::fromLatin1("%1 (Codec %2)").arg(sizev, sizec);

        if (statistics.video.frames == 0) valuesVideo << "";
        else valuesVideo << QString::number(statistics.video.frames);

        if (statistics.video.frame_rate == 0) valuesVideo << "";
        else valuesVideo << QString::fromLatin1("%1 / %2").arg(QString::number(statistics.video.frame_rate, 'f', 2),
                                                               QString::number(statistics.video.frame_rate, 'f', 2));
    }

    auto bitdepth = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("BitDepth")));
    if (bitdepth.isEmpty()) valuesVideo << "";
    else valuesVideo << bitdepth + " bits";

    valuesVideo << QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("ChromaSubsampling")));

    qDebug() << QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, 0, __T("BitDepth")));
    qDebug() << QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Audio, 0, __T("SamplingRate")));


    Q_EMIT baseValues(values, valuesVideo);

//    std::cout << QString::fromStdWString(MI.Inform()).toStdString();


//    qDebug() << QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("Format")));
//    qDebug() << QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("Format")));
//    qDebug() << QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("Format")));
//    qDebug() << QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("Format")));
//    qDebug() << QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("Format")));
//    qDebug() << QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("Format")));
//    qDebug() << QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_Video, 0, __T("Format")));


    MI.Close();

    DG_T << "Finalizando o thread " << QThread::currentThreadId();
    Q_EMIT finished();
}


/** Reescrevendo o texto de alguns formatos */
QString StatisticsWorker::setFormat(const QString &format) {
    switch (Hash::hash(format.toStdString())) {
        case "MPEG-4"_hash:   return "mp4 - MPEG-4 (Moving Picture Expert Group 4)";
        case "Matroska"_hash: return "mkv - Matroska (Matroska Video)";
        case "WebM"_hash:     return "webm - WebM (Web Media)";
        default:              return format;
    }
}


/** Definindo o tamanho do arquivo de mídia */
QString StatisticsWorker::convertByte(auto byte) {
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
        return QString::fromLatin1("%1 (%2)").arg(hr + ":" + vr, ratio);
    }
}

#undef DG_T
