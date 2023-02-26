#ifndef OMPLAYER_WORKER_H
#define OMPLAYER_WORKER_H

#include <QObject>

class Worker : public QObject {
Q_OBJECT

public:
    explicit Worker(QObject *parent = nullptr);

    void requestWork();
    void setFile(const QString &val) { file = val; };
    void noEmit(bool val) { emitValue = val; };


#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"

Q_SIGNALS:
    void workRequested();
    void valueMD5(const QString &value);
    void finished();

#pragma clang diagnostic pop


public Q_SLOTS:
    void doWork();

private:
    QString file{};
    bool emitValue{false};
};

#endif //OMPLAYER_WORKER_H
