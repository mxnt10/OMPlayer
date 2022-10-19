#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QMutex>

class Worker : public QObject {
Q_OBJECT

public:
    explicit Worker(QObject *parent = nullptr);
    void requestWork();

signals:
    void workRequested();
    void valueChanged(const QStringList &value);
    void finished();

public slots:
    void doWork();
};

#endif // WORKER_H
