#include <QThread>
#include <Utils>

#include "LoopButton.hpp"

#define DG_T qDebug().nospace() << GRE << "(" << RED << "Thread" << GRE << ")" << RED << "::" << RDL


/**********************************************************************************************************************/


/** Construtor */
Loop::Loop(QObject *parent) : QObject(parent) {}


/**********************************************************************************************************************/


/** Função para chamar o thread */
void Loop::requestWork() {
    end = false;
    Q_EMIT workRequested();
}


/** Thread para obter o loop do botão */
void Loop::doLoop() {
    DG_T << "Iniciando o loop " << QThread::currentThreadId();

    Q_EMIT looping();
    QThread::msleep(500);
    while (!end) {
        Q_EMIT looping();
        QThread::msleep(200);
    }
}


/** Finalizando Thread de loop */
void Loop::End() {
    end = true;

    DG_T << "Finalizando o loop " << QThread::currentThreadId();
    Q_EMIT finished();
}

#undef DG_T
