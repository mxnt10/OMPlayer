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
void StatisticsView::setMd5(const QString &md5) { baseItems[6]->setData(1, Qt::DisplayRole, md5); }


/** setar nas informações do formato dos arquivos multimídia */
void StatisticsView::setFormat(const QString &format) { emit emitFormat(format); }


/** Especificações das informações básicas */
QStringList StatisticsView::getBaseInfoKeys() {
    return { tr("File Path"),     // 0
             tr("File Name"),      // 1
             tr("File Size"),       // 2
             tr("Format"),           // 3
             tr("Overall Bit Rate"), // 4
             tr("Duration"),         // 5
             tr("Md5Sum") };         // 6
}


/** Especificações para informações de vídeo */
QStringList StatisticsView::getVideoInfoKeys() {
    return { tr("Codec"),        // 0
             tr("Decoder"),      // 1
             tr("Bit Rate"),     // 2
             tr("Aspect Ratio"),  // 3
             tr("Size"),           // 4
             tr("Frames"),          // 5
             tr("Frame Rate"),       // 6
             tr("Bit Depth"),         // 7
             tr("Chroma Subsampling"), // 8
             tr("Pixel Format") };     // 9
}


/** Especificações para informações de áudio */
QStringList StatisticsView::getAudioInfoKeys() {
    return { tr("Codec"),      // 0
             tr("Decoder"),     // 1
             tr("Bit Rate"),     // 2
             tr("Sample Rate"),   // 3
             tr("Sample Format"), // 4
             tr("Channels"),      // 5
             tr("Frames"),        // 6
             tr("Frame Size"),    // 7
             tr("Frame Rate"),    // 8
             tr("Bit Depth"),     // 9
             tr("VU Meter") };    // 10
}


/** Especificações para informações de metadados */
QStringList StatisticsView::getMetaDataKeys() {
    return { tr("Title"),  // 0
             tr("Artist"), // 1
             tr("Album"),  // 2
             tr("Genre"),  // 3
             tr("Track"),  // 4
             tr("Date"),   // 5
             tr("Link") }; // 6
}


/** Setando as informações de mídia */
void StatisticsView::setItemValues(const QStringList &values, const QStringList &valuesVideo,
                                   const QStringList &valuesAudio) {
    QStringList v = values;
    int i = 0;

    /** Atualizando informações gerais */
    foreach(QTreeWidgetItem* it, baseItems) {
        if (i == v.count()) break; // Evita erros de segmentação
        if (it->data(1, Qt::DisplayRole) != v.at(i)) it->setData(1, Qt::DisplayRole, v.at(i));
        ++i;
    }

    v = valuesVideo;
    i = 0;

    /** Atualizando informações de vídeo */
    foreach(QTreeWidgetItem* it, videoItems) {
        if (i == v.count()) break; // Evita erros de segmentação
        if (it->data(1, Qt::DisplayRole) != v.at(i)) it->setData(1, Qt::DisplayRole, v.at(i));
        ++i;
    }

    v = valuesAudio;
    i = 0;

    /** Atualizando informações de áudio */
    foreach(QTreeWidgetItem* it, audioItems) {
        if (i == v.count()) break; // Evita erros de segmentação
        if (it->data(1, Qt::DisplayRole) != v.at(i)) it->setData(1, Qt::DisplayRole, v.at(i));
        ++i;
    }

    settaginfos();
    visibility();
    setSize();
}


/** Resetando as informações de estatísticas */
void StatisticsView::resetValues() {
    QList<QList<QTreeWidgetItem*>> item = {baseItems, videoItems, audioItems, metadata};
    foreach(QList<QTreeWidgetItem *> list, item)
    foreach(QTreeWidgetItem* it, list) it->setData(1, Qt::DisplayRole, "");
    statistics = QtAV::Statistics();
    settaginfos();
    visibility();
    setSize();
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

    QVariantList v = getMetaDataValues(s);
    int i = 0;

    /** Atualizando informações de metadados */
    foreach(QTreeWidgetItem* it, metadata) {
        if (it->data(1, Qt::DisplayRole) != v.at(i)) it->setData(1, Qt::DisplayRole, v.at(i));
        ++i;
    }

    if(!s.video_only.pix_fmt.isEmpty()) videoItems[9]->setData(1, Qt::DisplayRole, s.video_only.pix_fmt);
    baseItems[6]->setData(1, Qt::DisplayRole, tr("Calculating..."));

    if (thread->isRunning()) thread->quit();
    if (thread2->isRunning()) thread2->quit();
    statisticsworker->setFile(s.url, s);
    statisticsworker->requestWork();
    worker->setFile(s.url);
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
    QList<TreeView*> view = {view2, view3, view4};
    foreach(TreeView *t, view) for (int i = 0; i < t->topLevelItemCount(); ++i) t->topLevelItem(i)->setHidden(false);

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
        int i = 0;
        foreach(QTreeWidgetItem* it, audioItems) {
            if (it->data(1, Qt::DisplayRole).toString().isEmpty()) view3->topLevelItem(i)->setHidden(true);
            i++;
        }
    }
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
    QList<TreeView*> view = {view1, view2, view3, view4};
    foreach(TreeView *it, view) it->header()->setStretchLastSection(false);

    int csize = view1->size();
    if (csize < view2->size()) csize = view2->size();
    if (csize < view3->size()) csize = view3->size();
    if (csize < view4->size()) csize = view4->size();

    foreach(TreeView *it, view) it->header()->setStretchLastSection(true);

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
    vuright = QStringLiteral("%1").arg(value * (-1), 2, 10, QLatin1Char('0'));
    audioItems[10]->setData(1, Qt::DisplayRole, QString::fromLatin1("( -%1 dB ) ( -%2 dB )").arg(vuleft, vuright));
}


/** Setando informações do dB esquerdo */
void StatisticsView::setLeftDB(int value) {
    vuleft = QStringLiteral("%1").arg(value * (-1), 2, 10, QLatin1Char('0'));
    audioItems[10]->setData(1, Qt::DisplayRole, QString::fromLatin1("( -%1 dB ) ( -%2 dB )").arg(vuleft, vuright));
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

    /** Duração atual */
    if (!statistics.url.isEmpty())
        baseItems[5]->setData(1, Qt::DisplayRole, QString::fromLatin1("%1 / %2").arg(
                ctime, statistics.duration.toString(QString::fromLatin1("HH:mm:ss"))));
}


/** Prevenindo fechamento sem onClose() */
void StatisticsView::closeEvent(QCloseEvent *event) {
    if (!onclose) {
        event->ignore();
        onClose();
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
