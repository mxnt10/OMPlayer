#ifndef OMPLAYER_WORKER_H
#define OMPLAYER_WORKER_H

#include <QObject>

class Worker : public QObject {
Q_OBJECT

public:
    explicit Worker(QObject *parent = nullptr);

    void requestWork() { Q_EMIT workRequested(); };
    void setFile(const QString &val) { file = val; };

Q_SIGNALS:
    void workRequested();
    void valueMD5(const QString &value);
    void finished();

public Q_SLOTS:
    void doWork();

private:
    QString file{};
};

#endif //OMPLAYER_WORKER_H