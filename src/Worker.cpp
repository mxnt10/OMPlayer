#include <QFileDialog>
#include <QStandardPaths>
#include <QThread>
#include <Utils>

#include <QLoggingCategory>

#include "Worker.h"


/**********************************************************************************************************************/


/** Construtor */
Worker::Worker(QObject *parent) : QObject(parent) {}


/**********************************************************************************************************************/


/** Função para chamar o thread */
void Worker::requestWork() {
    qDebug().nospace() << GRE << "(" << RED << "Thread" << GRE << ")" << RED << "::" << RDL
    << "Solicitando início do thread " << QThread::currentThreadId();
    emit workRequested();
}


/** Função que será usado para processar opreações em thread */
void Worker::doWork() {
    qDebug().nospace() << GRE << "(" << RED << "Thread" << GRE << ")" << RED << "::" << RDL
    << "Iniciando o thread " << QThread::currentThreadId();

    /** Hack para o mouse não ocultar no diálogo para abrir arquivos */
    for (int i = 0; i < 500; i++) arrowMouse();

    QStringList files = \
        QFileDialog::getOpenFileNames(
            nullptr, tr("Select multimedia files"),
            QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()),
            "Video Files (*.3gp *.3gpp *.m4v *.mp4 *.m2v *.mp2 *.mpeg *.mpg *.vob *.ogg *.ogv *.mov *.rmvb *.webm "
            "*.flv *.mkv *.wmv *.avi *.divx);;"
            "Audio Files (*.ac3 *.flac *.mid *.midi *.m4a *.mp3 *.opus *.mka *.wma *.wav);;"
            "3GPP Multimedia Files (*.3ga *.3gp *.3gpp);;3GPP2 Multimedia Files (*.3g2 *.3gp2 *.3gpp2);;"
            "AVI Video (*.avf *.avi *.divx);;Flash Video (*.flv);;Matroska Video (*.mkv);;"
            "Microsoft Media Format (*.wmp);;MPEG Video (*.m2v *.mp2 *.mpe *.mpeg *.mpg *.ts *.vob *.vdr);;"
            "MPEG-4 Video (*.f4v *.lrv *.m4v *.mp4);;OGG Video (*.ogg *.ogv);;"
            "QuickTime Video (*.moov *.mov *.qt *.qtvr);;RealMedia Format (*.rv *.rvx *.rmvb);;"
            "WebM Video (*.webm);;Windows Media Video (*.wmv);;"
            "AAC Audio (*.aac *.adts *.ass );;Dolby Digital Audio (*.ac3);;FLAC Audio (*.flac);;"
            "Matroska Audio (*.mka);;MIDI Audio (*.kar *.mid *.midi);;MPEG-4 Audio (*.f4a *.m4a);;"
            "MP3 Audio (*.mp3 *.mpga);;OGG Audio (*.oga *.opus *.spx);;Windows Media Audio (*.wma);;"
            "WAV Audio (*.wav);;WavPack Audio (*.wp *.wvp);;Media Playlist (*.m3u *.m3u8);;All Files (*);;"
        );

    if (!files.isEmpty()) emit valueChanged(files);

    qDebug().nospace() << GRE << "(" << RED << "Thread" << GRE << ")" << RED << "::" << RDL
    << "Finalizando o thread " << QThread::currentThreadId();
    emit finished();
}
