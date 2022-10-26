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

signals:
    void workRequested();
    void valueChanged(const QStringList &value);
    void valueMD5(const QString &value);
    void finished();

public slots:
    void doFiles();
    void doHash();

private:
    QString file{}, extensions{}, dir{};
};

#endif // WORKER_H
