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
Worker::Worker(QObject *parent, Worker::WORKER work) : QObject(parent), option(work) {
    Extensions e;
    extensions = tr("All Multimedia Files") + e.allPlayable().forFilter() + ";;" +
                 tr("Video and Audio Files") + e.multimedia().forFilter() + ";;" +
                 tr("Video") + e.video().forFilter() + ";;" +
                 tr("Audio") + e.audio().forFilter() + ";;" +
                 tr("Playlists") + e.playlist().forFilter() + ";;" +
                 tr("All files") + " (*.*)";
    dir = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath());
}


/**********************************************************************************************************************/


/** Função para chamar o thread */
void Worker::requestWork() {
    DG_T << "Solicitando início do thread " << QThread::currentThreadId();
    emit workRequested();
}


/** Função que será usado para processar opreações em thread */
void Worker::doWork() {
    DG_T << "Iniciando o thread " << QThread::currentThreadId();

    if (option == Worker::Add_Items) {
        /** Hack para o mouse não ocultar no diálogo para abrir arquivos */
        for (int i = 0; i < 500; i++) arrowMouse();

        QStringList files = QFileDialog::getOpenFileNames(nullptr, tr("Select multimedia files"), dir, extensions);
        if (!files.isEmpty()) emit valueChanged(files);
    }

    if (option == Worker::Md5Hash) {
        QString hash{Utils::setHash(file)};
        emit valueMD5(hash);
    }

    DG_T << "Finalizando o thread " << QThread::currentThreadId();
    emit finished();
}

#undef DG_T
