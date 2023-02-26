#ifndef OMPLAYER_STATISTICSWORKER_H
#define OMPLAYER_STATISTICSWORKER_H

#include <QObject>
#include <MediaInfoDLL.h>
#include <QtAV/Statistics.h>

class StatisticsWorker : public QObject {
Q_OBJECT

public:
    explicit StatisticsWorker(QObject *parent = nullptr);

    void setFile(const QString &val, const QtAV::Statistics &s = QtAV::Statistics(),
                 const QtAV::Statistics &d = QtAV::Statistics());
    void requestWork() { Q_EMIT workRequested(); };


#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"

Q_SIGNALS:
    void baseValues(const QStringList &values, const QStringList &valuesVideo, const QStringList &valuesAudio,
                    const QStringList &valuesDual, const QStringList &metadataval, const QString &format);
    void workRequested();
    void finished();

#pragma clang diagnostic pop


public Q_SLOTS:
    void doWork();

private:
    [[nodiscard]] QString setFormat(const QString &format) const;
    static QString setFormatInfo(const QString &finfo);
    static QString setCodecInfo(const QString &cinfo);
    QString convertAspectRatio(int x, int y);
    static QString convertByte(auto byte);
    static QString convertBit(auto bit);
    static QString convertHz(auto hz);

    QString file{}, rat{};
    QtAV::Statistics statistics{}, dualstatistics{};
    MediaInfoDLL::MediaInfo MI{};
};

#endif //OMPLAYER_STATISTICSWORKER_H
