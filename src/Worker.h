#ifndef WORKER_H
#define WORKER_H

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
    void doHash();
    void doFormat();

private:
    QString file{};
    MediaInfoDLL::MediaInfo MI{};
};

#endif // WORKER_H
