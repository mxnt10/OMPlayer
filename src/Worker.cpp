#include <QThread>
#include <Utils>

#include "Worker.hpp"

#define DG_T qDebug().nospace() << GRE << "(" << RED << "Thread" << GRE << ")" << RED << "::" << RDL


/**********************************************************************************************************************/


/** Construtor
 *
 * QThread::HighestPriority
 * QThread::LowestPriority
 *
 * */
Worker::Worker(QObject *parent) : QObject(parent) {}


/**********************************************************************************************************************/


/** Thread para obter o hash md5 de um arquivo */
void Worker::doWork() {
    DG_T << "Iniciando o thread " << QThread::currentThreadId();

    QString hash{Utils::setHash(file)};
    if (!emitValue) Q_EMIT valueMD5(hash);

    DG_T << "Finalizando o thread " << QThread::currentThreadId();
    Q_EMIT finished();
}


/** Solicitando a execução do thread */
void Worker::requestWork() {
    emitValue = false;
    Q_EMIT workRequested();
}

#undef DG_T
