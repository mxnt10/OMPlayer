#include <QCloseEvent>
#include <QHeaderView>
#include <QLayout>
#include <QTimer>
#include <QTimerEvent>
#include <Utils>

#include "StatisticsView.h"


/**********************************************************************************************************************/


/** Interface principal para visualizar as informações de mídia */
StatisticsView::StatisticsView(QWidget *parent) : QDialog(parent) {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setModal(true);
    setFocus();


    /** Efeito de transparência funcional. O setWindowOpacity() não rola. */
    effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(0);
    setGraphicsEffect(effect);
    animation = new QPropertyAnimation(effect, "opacity");


    /** Usando multithread para buscar arquivos */
    thread = new QThread();
    worker = new Worker();
    worker->moveToThread(thread);
    connect(worker, &Worker::valueMD5, this, &StatisticsView::setMd5);
    connect(worker, &Worker::valueFormat, this, &StatisticsView::setFormat);
    connect(thread, &QThread::started, worker, &Worker::doWork);
    connect(worker, &Worker::workRequested, [this](){ thread->start(); });
    connect(worker, &Worker::finished, [this](){ thread->quit(); });


    /** Buscando estatísticas com mediainfolib */
    thread2 = new QThread();
    statisticsworker = new StatisticsWorker();
    statisticsworker->moveToThread(thread2);
    connect(statisticsworker, &StatisticsWorker::baseValues, this, &StatisticsView::setItemValues);
    connect(thread2, &QThread::started, statisticsworker, &StatisticsWorker::doWork);
    connect(statisticsworker, &StatisticsWorker::workRequested, [this](){ thread2->start(); });
    connect(statisticsworker, &StatisticsWorker::finished, [this](){ thread2->quit(); });


    /** Iniciando as informações dos itens */
    initItems(&baseItems, getBaseInfoKeys());
    initItems(&videoItems, getVideoInfoKeys());
    initItems(&audioItems, getAudioInfoKeys());
    initItems(&metadata, getMetaDataKeys());


    /** Criando visualizador para as informações */
    view1 = new TreeView(baseItems);
    view2 = new TreeView(videoItems);
    view3 = new TreeView(audioItems);
    view4 = new TreeView(metadata);


    /** Botão para fechar a janela */
    closebtn = new Button(Button::Default, 32, "apply");
    connect(closebtn, &Button::pressed, this, &StatisticsView::onClose);


    /** Layout para as abas */
    auto *infogeral = new QWidget();
    auto *geral= new QVBoxLayout(infogeral);
    geral->addWidget(view1);
    auto *infovideo = new QWidget();
    auto *video = new QVBoxLayout(infovideo);
    video->addWidget(view2);
    auto *infoaudio = new QWidget();
    auto *audio = new QVBoxLayout(infoaudio);
    audio->addWidget(view3);
    auto *metadataw = new QWidget();
    auto *mdata = new QVBoxLayout(metadataw);
    mdata->addWidget(view4);

    /** Organização por abas */
    tab = new QTabWidget();
    tab->addTab(infogeral, tr("General"));
    tab->addTab(infovideo, tr("Video"));
    tab->addTab(infoaudio, tr("Audio"));
    tab->addTab(metadataw, tr("MedaData"));
    tab->setStyleSheet(Utils::setStyle("tab"));


    /** Botões que servirão de tag info */
    screen = new Button(Button::Tag, 36);
    ratio = new Button(Button::Tag, 36);


    /** Layout para as tag infos */
    auto *tags = new QGridLayout();
    tags->addWidget(screen, 0, 0);
    tags->addWidget(ratio, 0, 1);


    /** Layout para organização das configurações */
    auto *fore = new QGridLayout();
    fore->setMargin(10);
    fore->addWidget(tab, 0, 0, 1, 2);
    fore->addLayout(tags, 1, 0, LEFT);
    fore->addWidget(closebtn, 1, 1, RIGHT);


    /** Fundo do diálogo de informações */
    auto *found = new QWidget();
    found->setStyleSheet(Utils::setStyle("widget"));


    /** Layout principal */
    auto *layout = new QGridLayout();
    layout->setMargin(0);
    layout->addWidget(found, 0, 0);
    layout->addLayout(fore, 0, 0);
    setLayout(layout);


    /** Ajuste inicial */
    visibility();
    setSize();
}


/** Destrutor */
StatisticsView::~StatisticsView() {
    thread->quit();
    thread2->quit();
    delete thread;
    delete thread2;
    delete worker;
    delete statisticsworker;
}


/**********************************************************************************************************************/


/** setar nas informações o hash MD5 dos arquivos multimídia */
void StatisticsView::setMd5(const QString &md5) { MD5->setData(1, Qt::DisplayRole, md5); }


/** setar nas informações o hash MD5 dos arquivos multimídia */
void StatisticsView::setFormat(const QString &format) {
    FORMAT->setData(1, Qt::DisplayRole, format);
    if (QString::compare(FORMAT->text(1), "") != 0) emit emitFormat(format);
}


/** Emissão para fechar a janela */
void StatisticsView::onClose() {
    qDebug("%s(%sStatisticsView%s)%s::%sFechando o diálogo de informações ...\033[0m", GRE, RED, GRE, RED, CYA);
    onclose = true;
    Utils::fadeDiag(animation, 1, 0);
    connect(animation, &QPropertyAnimation::finished, [this](){
        if (!onclose) return;
        Q_EMIT emitclose();
        this->close();
    });
}


/** Especificações das informações básicas */
QStringList StatisticsView::getBaseInfoKeys() {
    return QStringList()
        << tr("File Path")
        << tr("File Name")
        << tr("File Size")
        << tr("Format")
        << tr("Bit Rate")
        << tr("Duration")
        << tr("Md5Sum");
}


/** Especificações para informações de vídeo */
QStringList StatisticsView::getVideoInfoKeys() {
    return QStringList()
        << tr("Codec")
        << tr("Decoder")
        << tr("Bit Rate")
        << tr("Aspect Ratio")
        << tr("Size")
        << tr("Frames")
        << tr("Frame Rate")
        << tr("Pixel Format")
        << tr("GOP Size");
}


/** Especificações para informações de áudio */
QStringList StatisticsView::getAudioInfoKeys() {
    return QStringList()
        << tr("Codec")
        << tr("Decoder")
        << tr("Bit Rate")
        << tr("Sample Rate")
        << tr("Sample Format")
        << tr("Channels")
        << tr("Frames")
        << tr("Frame Size")
        << tr("Frame Rate")
        << tr("VU Meter");
}


/** Especificações para informações de metadados */
QStringList StatisticsView::getMetaDataKeys() {
    return QStringList()
            << tr("Title")
            << tr("Artist")
            << tr("Album")
            << tr("Genre")
            << tr("Track")
            << tr("Date")
            << tr("Link");
}


/** Informações básicas de mídia */
QVariantList StatisticsView::getBaseInfoValues(const QtAV::Statistics& s) {
    QString rate, duration;
    if (!s.url.isEmpty()) {
        rate = QString::number(s.bit_rate/1000).append(QString::fromLatin1(" Kb/s"));
        duration = QString::fromLatin1("%1 / %2").arg(ctime, s.duration.toString(QString::fromLatin1("HH:mm:ss")));
    }

    return QVariantList()
        << QString(s.url).remove(QRegExp("\\/(?:.(?!\\/))+$"))
        << QString(s.url).remove(QRegExp("\\/.+\\/"))
        << fsize
        << QString()
        << rate
        << duration
        << QString();
}


/** Informações de vídeo */
QVariantList StatisticsView::getVideoInfoValues(const QtAV::Statistics& s) {
    QString sizev = QString::fromLatin1("%1x%2").arg(s.video_only.width).arg(s.video_only.height);
    QString sizec = QString::fromLatin1("%1x%2").arg(s.video_only.coded_width).arg(s.video_only.coded_height);

    return QVariantList()
        << QString::fromLatin1("%1 (%2)").arg(s.video.codec, s.video.codec_long)
        << QString::fromLatin1("%1 (%2)").arg(s.video.decoder, s.video.decoder_detail)
        << QString::number(s.video.bit_rate/1000).append(QString::fromLatin1(" Kb/s"))
        << QString::number(((double)s.video_only.width / s.video_only.height))
        << QString::fromLatin1("%1 (Codec: %2)").arg(sizev, sizec)
        << s.video.frames
        << QString::fromLatin1("%1 / %2").arg(QString::number(s.video.frame_rate, 'f', 2),
                                              QString::number(s.video.frame_rate, 'f', 2))
        << s.video_only.pix_fmt
        << s.video_only.gop_size;
}


/** Informações de áudio */
QVariantList StatisticsView::getAudioInfoValues(const QtAV::Statistics& s) {
    return QVariantList()
        << QString::fromLatin1("%1 (%2)").arg(s.audio.codec, s.audio.codec_long)
        << QString::fromLatin1("%1 (%2)").arg(s.audio.decoder, s.audio.decoder_detail)
        << QString::number(s.audio.bit_rate/1000).append(QString::fromLatin1(" Kb/s"))
        << QString::number(s.audio_only.sample_rate).append(QString::fromLatin1(" Hz"))
        << s.audio_only.sample_fmt
        << QString::fromLatin1("%1 (Layout: %2)").arg(QString::number(s.audio_only.channels), s.audio_only.channel_layout)
        << s.audio.frames
        << s.audio_only.frame_size
        << s.audio.frame_rate
        << QString();
}


/** Informações de metadados */
QVariantList StatisticsView::getMetaDataValues(const QtAV::Statistics& s) {
    QStringList infos{"title", "artist", "album", "genre", "track", "date", "purl"};
    QVariantList values{};
    QStringList keys{s.metadata.keys()};

    for(const QString &inf : infos) {
        if (keys.contains(inf, Qt::CaseInsensitive)) {
            if (inf == "date") {
                QDate dt = QDate::fromString(s.metadata.value(keys.filter(inf, Qt::CaseInsensitive)[0]), "yyyyMMdd");
                values.append(dt.toString("yyyy-MM-dd"));
            } else if (inf == "track") {
                QStringList tm = keys.filter("track", Qt::CaseInsensitive);
                if (tm.contains("tracktotal", Qt::CaseInsensitive)) {

                    QString tot = s.metadata.value(tm.filter("tracktotal", Qt::CaseInsensitive)[0]);
                    tm.removeOne(keys.filter("tracktotal", Qt::CaseInsensitive)[0]);
                    QString tr = s.metadata.value(tm.filter("track", Qt::CaseInsensitive)[0]);

                    if (tot == "") values.append(s.metadata.value(keys.filter(inf, Qt::CaseInsensitive)[0]));
                    else values.append(QString::fromLatin1("%1 / %2").arg(tr, tot));

                } else values.append(s.metadata.value(keys.filter(inf, Qt::CaseInsensitive)[0]));
            } else values.append(s.metadata.value(keys.filter(inf, Qt::CaseInsensitive)[0]));
        } else values.append("");
    }

    return values;
}


/** Função para coletar as informações básicas dos items */
void StatisticsView::initItems(QList<QTreeWidgetItem *> *items, const QStringList &itemlist) {
    QTreeWidgetItem *item;
    foreach(const QString& key, itemlist) {
        item = new QTreeWidgetItem(0);
        item->setData(0, Qt::DisplayRole, key);
        items->append(item);
    }
}


/** Função usada para setar as estatísticas dos itens atuais */
void StatisticsView::setStatistics(const QtAV::Statistics& s) {
    qDebug("%s(%sStatisticsView%s)%s::%sAtualizando informações para %s ...\033[0m", GRE, RED, GRE, RED, UPD,
           STR(QString(s.url).remove(QRegExp("\\/.+\\/"))));

    qDebug() << s.metadata.keys();

    /** Redefinindo informações */
    resetValues();
    ctime = "00:00:00";
    statistics = s;
    QString byte{"B"};
    QStringList byteit{"KiB", "MiB", "GiB"};
    QFileInfo file{s.url};
    auto size = (double)file.size();

    /** Definindo o tamanho do arquivo de mídia */
    while (size > 1024) {
        size = size / 1024;
        byte = byteit[i];
        i++;
    }

    fsize = QString::fromLatin1("%1 %2 (%3)").arg(QString::number(size, 'f', 2), byte).arg(file.size());
    QVariantList v = getBaseInfoValues(s);
    i = 0;

    /** Atualizando informações gerais */
    foreach(QTreeWidgetItem* it, baseItems) {
        if (it->data(1, Qt::DisplayRole) != v.at(i)) it->setData(1, Qt::DisplayRole, v.at(i));
        ++i;
    }

    v = getVideoInfoValues(s);
    i = 0;

    /** Atualizando informações de vídeo */
    foreach(QTreeWidgetItem* it, videoItems) {
        if (it->data(1, Qt::DisplayRole) != v.at(i)) it->setData(1, Qt::DisplayRole, v.at(i));
        ++i;
    }

    v = getAudioInfoValues(s);
    i = 0;

    /** Atualizando informações de áudio */
    foreach(QTreeWidgetItem* it, audioItems) {
        if (it->data(1, Qt::DisplayRole) != v.at(i)) it->setData(1, Qt::DisplayRole, v.at(i));
        ++i;
    }

    v = getMetaDataValues(s);
    i = 0;

    /** Atualizando informações de metadados */
    foreach(QTreeWidgetItem* it, metadata) {
        if (it->data(1, Qt::DisplayRole) != v.at(i)) it->setData(1, Qt::DisplayRole, v.at(i));
        ++i;
    }

    /** Buscando o maior comprimento para a janela */
    view1->header()->setStretchLastSection(false);
    view2->header()->setStretchLastSection(false);
    view3->header()->setStretchLastSection(false);
    view4->header()->setStretchLastSection(false);

    settaginfos();
    visibility();
    int csize = view1->size();
    if (csize < view2->size()) csize = view2->size();
    if (csize < view3->size()) csize = view3->size();
    if (csize < view4->size()) csize = view4->size();

    view1->header()->setStretchLastSection(true);
    view2->header()->setStretchLastSection(true);
    view3->header()->setStretchLastSection(true);
    view4->header()->setStretchLastSection(true);

    csize = csize + 40;
    qDebug("%s(%sStatisticsView%s)%s::%sAjustando comprimento de infoview em %i ...\033[0m",
           GRE, RED, GRE, RED, BLU, csize);

    this->setMinimumSize(csize, 350);
    this->resize(csize, this->height());

    /** Buscando informações em segundo plano */
    worker->setFile(s.url);
    MD5->setData(1, Qt::DisplayRole, tr("Calculating..."));
    FORMAT->setData(1, Qt::DisplayRole, tr("Searching..."));

    if (thread->isRunning()) thread->quit();
    worker->requestWork();
}


/** Função para setar a duração atual da mídia reproduzida */
void StatisticsView::setCurrentTime(int current) {
    ctime = QTime(0, 0, 0).addMSecs(current).toString(QString::fromLatin1("HH:mm:ss"));
}


/** Verificar o que precisa ser oculto */
void StatisticsView::visibility(){
    /** Redefinindo visualização */
    for (int i = 0; i < 4; i++) tab->setTabVisible(i, false);
    for (int i = 0; i < 4; i++) tab->setTabVisible(i, true);
    for (int i = 0; i < view2->topLevelItemCount(); ++i) view2->topLevelItem(i)->setHidden(false);
    for (int i = 0; i < view3->topLevelItemCount(); ++i) view3->topLevelItem(i)->setHidden(false);
    for (int i = 0; i < view4->topLevelItemCount(); ++i) view4->topLevelItem(i)->setHidden(false);

    /** Verificando status de vídeo e áudio e o que precisa ser oculto */
    if (!statistics.video.available) tab->setTabVisible(1, false);
    else {
        int i = 0;
        foreach(QTreeWidgetItem* it, videoItems) {
            if (it->data(1, Qt::DisplayRole).toString().isEmpty()) view2->topLevelItem(i)->setHidden(true);
            i++;
        }
    }
    if (!statistics.audio.available) tab->setTabVisible(2, false);
    else {
        if (statistics.audio.bit_rate == 0) view3->topLevelItem(2)->setHidden(true);
        if (statistics.audio.frames == 0) view3->topLevelItem(6)->setHidden(true);
        if (statistics.audio_only.frame_size == 0) view3->topLevelItem(7)->setHidden(true);
        if (statistics.audio.frame_rate == 0) view3->topLevelItem(8)->setHidden(true);
    }

    /** Verificando visibilidade dos metadados */
    if (statistics.metadata.isEmpty()) tab->setTabVisible(3, false);
    else {
        bool visibility{false};
        QVariantList v = getMetaDataValues(statistics);
        int i = 0;

        foreach(QVariant var, v) {
            if (!var.toString().isEmpty()) visibility = true;
            else view4->topLevelItem(i)->setHidden(true);
            ++i;
        }

        if (!visibility) tab->setTabVisible(3, false);
    }
}


/**
 * Resoluções de tela:
 *
 * FUHD|8k : 7680x4320 : 4320p
 *
 *  8192x4320 : 17:9
 *  8192x5120 : 16:10
 * 10080x4320 : 21:9
 *
 * UHD+|5k : 5120x2880 : 2880p
 *
 * 5120x1440 : 32∶9
 * 5120x2160 : 64:27
 * 5120x2560 : 18∶9
 * 4800x2700 : 16∶9
 * 5120x2700 : 17∶9
 * 5120x3200 : 16∶10
 * 5120x3840 : 4∶3
 * 5120x4096 : 5∶4
 *
 * UHD|4k : 3840x2160 : 2160p
 *
 * 4096x2160 : 21:9
 * 4096x1714
 * 3996x2160
 * 3656x2664
 *
 * 2K : 2048x1080 : 2048x858 : 1998x1080
 *
 * QHD : 2560x1440 : 1440p
 * FHD : 1920x1080 : 1080p
 * HD  : 1280x720  : 720p
 *
 * Função que seta as informações de mídia em ícones */
void StatisticsView::settaginfos() {
    ratio->setVisible(true);

    if (videoItems[3]->data(1, Qt::DisplayRole).toString().split(' ')[0] == "16:9")
        Utils::changeIcon(ratio, "ratio169");
    else if (videoItems[3]->data(1, Qt::DisplayRole).toString().split(' ')[0] == "4:3")
        Utils::changeIcon(ratio, "ratio43");
    else ratio->setVisible(false);

    int w = statistics.video_only.width;
    int h = statistics.video_only.height;
    screen->setVisible(true);
    bool change = false;

    for (int i : fuhdw) if (i == w) for (int j : fuhdh) if (j == h) {
        Utils::changeIcon(screen, "screen8k");
        change = true;
    }

    if (!change) for (int i : uhdpw) if (i == w) for (int j : uhdph) if (j == h) {
        Utils::changeIcon(screen, "screen5k");
        change = true;
    }

    if (!change) for (int i : uhdw) if (i == w) for (int j : uhdh) if (j == h) {
        Utils::changeIcon(screen, "screen4k");
        change = true;
    }

    if (!change && w == 2560 && h == 1440) {
        Utils::changeIcon(screen, "screenqhd");
        change = true;
    }

    if (!change && w == 1920 && h == 1080) {
        Utils::changeIcon(screen, "screenfhd");
        change = true;
    }

    if (!change && w == 1280 && h == 720) {
        Utils::changeIcon(screen, "screenhd");
        change = true;
    }

    if (!change) screen->setVisible(false);
}


/** Buscando o maior comprimento para a janela */
void StatisticsView::setSize() {
    view1->header()->setStretchLastSection(false);
    view2->header()->setStretchLastSection(false);
    view3->header()->setStretchLastSection(false);
    view4->header()->setStretchLastSection(false);

    int csize = view1->size();
    if (csize < view2->size()) csize = view2->size();
    if (csize < view3->size()) csize = view3->size();
    if (csize < view4->size()) csize = view4->size();

    view1->header()->setStretchLastSection(true);
    view2->header()->setStretchLastSection(true);
    view3->header()->setStretchLastSection(true);
    view4->header()->setStretchLastSection(true);

    csize = csize + 40;
    qDebug("%s(%sStatisticsView%s)%s::%sAjustando comprimento de infoview em %i ...\033[0m",
           GRE, RED, GRE, RED, BLU, csize);

    this->setMinimumSize(csize, 350);
    this->resize(csize, this->height());
}


/** Função para alterar o ícone do botão */
void StatisticsView::changeIcons() {
    Utils::changeIcon(closebtn, "apply");
    settaginfos();
}


/** Setando informações do dB direito */
void StatisticsView::setRightDB(int value) {
    vuright = value;
    audioItems[9]->setData(1, Qt::DisplayRole, QString::fromLatin1("%1 dB\n%2 dB").arg(vuleft).arg(vuright));
}


/** Setando informações do dB esquerdo */
void StatisticsView::setLeftDB(int value) {
    vuleft = value;
    audioItems[9]->setData(1, Qt::DisplayRole, QString::fromLatin1("%1 dB\n%2 dB").arg(vuleft).arg(vuright));
}


/**********************************************************************************************************************/


/** Evento para finalizar o temporizador */
void StatisticsView::hideEvent(QHideEvent *event) {
    killTimer(timer);
    QDialog::hideEvent(event);
}


/** Evento para iniciar o temporizador */
void StatisticsView::showEvent(QShowEvent *event) {
    onclose = false;
    timer = startTimer(1000);
    Utils::fadeDiag(animation, 0, 1);
    QDialog::showEvent(event);
}


/** Evento de temporização */
void StatisticsView::timerEvent(QTimerEvent *event) {
    if (event->timerId() != timer) return;

    if (statistics.video.frame_rate != 0)
        videoItems[6]->setData(1, Qt::DisplayRole, QString::fromLatin1("%1 / %2").arg(
                QString::number(statistics.video.frame_rate, 'f', 2),
                QString::number(statistics.video_only.currentDisplayFPS(),'f', 2)));

    if (!statistics.url.isEmpty())
        baseItems[5]->setData(1, Qt::DisplayRole, QString::fromLatin1("%1 / %2").arg(
                ctime, statistics.duration.toString(QString::fromLatin1("HH:mm:ss"))));
}


/** Prevenindo fechamento sem onClose() */
void StatisticsView::closeEvent(QCloseEvent *event) {
    if (!onclose) {
        QTimer::singleShot(10, this, &StatisticsView::onClose);
        return;
    } else event->accept();
}


/** Corrigindo fechamento do diálogo com Escape */
void StatisticsView::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Escape) {
        onClose();
        return;
    }
    QDialog::keyPressEvent(event);
}
