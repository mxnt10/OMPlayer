#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QMutex>

class Worker : public QObject {
Q_OBJECT

public:
    enum WORKER {Default = 0, Add_Items = 1, Md5Hash = 2, Update_Version = 3};
    explicit Worker(QObject *parent = nullptr, Worker::WORKER work = Worker::Default);
    void requestWork();

    void setFile(const QString &val) { file = val; };

signals:
    void workRequested();
    void valueChanged(const QStringList &value);
    void valueMD5(const QString &value);
    void finished();

public slots:
    void doWork();

private:
    QString file{}, extensions{}, dir{};
    Worker::WORKER option{Worker::Default};
};

#endif // WORKER_H
