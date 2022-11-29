#include <QLoggingCategory>
#include <QStandardPaths>
#include <QThread>
#include <Utils>

#include "Worker.h"

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


/** Função para chamar o thread */
void Worker::requestWork() {
    DG_T << "Solicitando início do thread " << QThread::currentThreadId();
    Q_EMIT workRequested();
}


/** Thread para obter o hash md5 de um arquivo */
void Worker::doHash() {
    DG_T << "Iniciando o thread " << QThread::currentThreadId();

    QString hash{Utils::setHash(file)};
    Q_EMIT valueMD5(hash);

    DG_T << "Finalizando o thread " << QThread::currentThreadId();
    Q_EMIT finished();
}

void Worker::doFormat() {
    DG_T << "Iniciando o thread " << QThread::currentThreadId();

    MI.Open(file.toStdWString()); //TODO: thread.
    QString format = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_General, 0, __T("Format"),
                                                    MediaInfoDLL::Info_Text, MediaInfoDLL::Info_Name));
    MI.Close();
    Q_EMIT valueFormat(format);

    DG_T << "Finalizando o thread " << QThread::currentThreadId();
    Q_EMIT finished();
}

#undef DG_T
