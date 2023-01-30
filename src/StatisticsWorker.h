#ifndef OMPLAYER_STATISTICSWORKER_H
#define OMPLAYER_STATISTICSWORKER_H

#include <QObject>
#include <MediaInfoDLL.h>
#include <QtAV/Statistics.h>

class StatisticsWorker : public QObject {
Q_OBJECT

public:
    explicit StatisticsWorker(QObject *parent = nullptr);

    void setFile(const QString &val, const QtAV::Statistics &s = QtAV::Statistics()) { file = val; statistics = s; };
    void requestWork() { Q_EMIT workRequested(); };

Q_SIGNALS:
    void baseValues(const QStringList &values, const QStringList &valuesVideo, const QStringList &valuesAudio,
                    const QStringList &valuesDual, const QStringList &metadataval,
                    const QString &format, int duration);
    void workRequested();
    void finished();

public Q_SLOTS:
    void doWork();

private:
    static QString setFormat(const QString &format);
    static QString convertByte(auto byte);
    static QString convertBit(auto bit);
    static QString convertAspectRatio(int x, int y);
    static QString convertHz(auto hz);

    QString file{};
    QtAV::Statistics statistics{};
    MediaInfoDLL::MediaInfo MI{};
};

#endif //OMPLAYER_STATISTICSWORKER_H
