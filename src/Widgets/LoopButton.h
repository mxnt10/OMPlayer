#ifndef OMPLAYER_LOOPBUTTON_H
#define OMPLAYER_LOOPBUTTON_H

#include <QObject>

class Loop : public QObject {
Q_OBJECT

public:
    explicit Loop(QObject *parent = nullptr);
    void requestWork();
    void End();

Q_SIGNALS:
    void workRequested();
    void looping();
    void finished();

public Q_SLOTS:
    void doLoop();

private:
    bool end{false};
};


#endif //OMPLAYER_LOOPBUTTON_H
