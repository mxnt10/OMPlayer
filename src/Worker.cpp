#include <QFileDialog>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QThread>
#include <Utils>

#include "Extensions.h"
#include "Worker.h"

#define DG_T qDebug().nospace() << GRE << "(" << RED << "Thread" << GRE << ")" << RED << "::" << RDL


/**********************************************************************************************************************/


/** Construtor */
Worker::Worker(QObject *parent) : QObject(parent) {
    Extensions e;
    extensions = tr("Multimedia Files") + e.multimedia().forFilter() + ";;" +
                 tr("Video") + e.video().forFilter() + ";;" +
                 tr("Audio") + e.audio().forFilter() + ";;" +
                 tr("Playlists") + e.playlist().forFilter() + ";;" +
                 tr("All Media Files") + e.allPlayable().forFilter() + ";;" +
                 tr("All Files") + " (*)";
    dir = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath());
}


/**********************************************************************************************************************/


/** Função para chamar o thread */
void Worker::requestWork() {
    DG_T << "Solicitando início do thread " << QThread::currentThreadId();
    Q_EMIT workRequested();
}


/** Thread para abrir para arquivos multimídia */
void Worker::doFiles() {
    DG_T << "Iniciando o thread " << QThread::currentThreadId();

    /** Hack para o mouse não ocultar no diálogo para abrir arquivos */
    for (int i = 0; i < 500; i++) arrowMouse();

    QStringList files = QFileDialog::getOpenFileNames(nullptr, tr("Select multimedia files"), dir, extensions);
    if (!files.isEmpty()) Q_EMIT valueChanged(files);

    DG_T << "Finalizando o thread " << QThread::currentThreadId();
    Q_EMIT finished();
}


/** Thread para obter o hash md5 de um arquivo */
void Worker::doHash() {
    DG_T << "Iniciando o thread " << QThread::currentThreadId();

    QString hash{Utils::setHash(file)};
    Q_EMIT valueMD5(hash);

    DG_T << "Finalizando o thread " << QThread::currentThreadId();
    Q_EMIT finished();
}

#undef DG_T
