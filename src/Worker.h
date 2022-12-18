#ifndef OMPLAYER_WORKER_H
#define OMPLAYER_WORKER_H

#include <QObject>
#include <MediaInfoDLL.h>

class Worker : public QObject {
Q_OBJECT

public:
    explicit Worker(QObject *parent = nullptr);
    void requestWork();

    void setFile(const QString &val) { file = val; };

Q_SIGNALS:
    void workRequested();
    void valueMD5(const QString &value);
    void valueFormat(const QString &value);
    void finished();

public Q_SLOTS:
    void doWork();

private:
    QString file{};
    MediaInfoDLL::MediaInfo MI{};
};

#endif //OMPLAYER_WORKER_H
