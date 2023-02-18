#ifndef OMPLAYER_LOOPBUTTON_HPP
#define OMPLAYER_LOOPBUTTON_HPP

#include <QObject>

class Loop : public QObject {
Q_OBJECT

public:
    explicit Loop(QObject *parent = nullptr);
    void requestWork();
    void End();


#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"

Q_SIGNALS:
    void workRequested();
    void looping();
    void finished();

#pragma clang diagnostic pop


public Q_SLOTS:
    void doLoop();

private:
    bool end{false};
};


#endif //OMPLAYER_LOOPBUTTON_HPP
