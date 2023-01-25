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
    Q_EMIT valueMD5(hash);

    DG_T << "Finalizando o thread " << QThread::currentThreadId();
    Q_EMIT finished();
}

#undef DG_T
