#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QMutex>

class Worker : public QObject {
Q_OBJECT

public:
    explicit Worker(QObject *parent = nullptr);
    void requestWork();

    void setFile(const QString &val) { file = val; };

Q_SIGNALS:
    void workRequested();
    void valueMD5(const QString &value);
    void finished();

public Q_SLOTS:
    void doHash();

private:
    QString file{};
};

#endif // WORKER_H
