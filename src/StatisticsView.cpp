#include <QHeaderView>
#include <QLayout>
#include <QScreen>
#include <Utils>

#include "StatisticsView.hpp"


/**********************************************************************************************************************/


/** Interface principal para visualizar as informações de mídia */
StatisticsView::StatisticsView(QWidget *parent) : Dialog(parent) {
    connect(this, &Dialog::activeShow, this, &StatisticsView::showDialog);
    connect(this, &Dialog::emitclose, [this](){ Q_EMIT onclose(); });


    /** Reprodutor invisível pra pegar as informações */
    st = new QtAV::AVPlayer();
    st->audio()->setMute(true);
    connect(st, &QtAV::AVPlayer::started, this, &StatisticsView::onStatistics);


    /** Usando multithread para buscar arquivos */
    thread = new QThread();
    worker = new Worker();
    worker->moveToThread(thread);
    connect(worker, &Worker::valueMD5, this, &StatisticsView::setMd5);
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
    initItems(&audioDual, getAudioInfoKeys());
    initItems(&metadata, getMetaDataKeys());
    itemTab << baseItems << videoItems << audioItems << audioDual << metadata;


    /** Criando visualizador para as informações */
    view1 = new TreeView(baseItems);
    view2 = new TreeView(videoItems);
    view3 = new TreeView(audioItems);
    view4 = new TreeView(audioDual);
    view5 = new TreeView(metadata);
    itemView << view1 << view2 << view3 << view4 << view5;


    /** Botão para fechar a janela */
    closebtn = new Button(Button::NormalBtn, 32, "apply");
    connect(closebtn, &Button::pressed, this, &Dialog::onClose);


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
    auto *infoaudio2 = new QWidget();
    auto *audio2 = new QVBoxLayout(infoaudio2);
    audio2->addWidget(view4);
    auto *metadataw = new QWidget();
    auto *mdata = new QVBoxLayout(metadataw);
    mdata->addWidget(view5);


    /** Organização por abas */
    tab = new QTabWidget();
    tab->addTab(infogeral, tr("General"));
    tab->addTab(infovideo, tr("Video"));
    tab->addTab(infoaudio, tr("Audio"));
    tab->addTab(infoaudio2, tr("Audio") + " #2");
    tab->addTab(metadataw, tr("MedaData"));
    tab->setStyleSheet(Utils::setStyle("tab"));
    for (int i = 1; i < 5; i++) tab->setTabVisible(i, false);


    /** Ícones que servirão de tag info */
    screen = new Label(LEFT, 36, 36);
    ratio = new Label(LEFT, 36, 36);


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
    setSize(StatisticsView::InitialSize);
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
void StatisticsView::setMd5(const QString &md5) {
    baseItems.at(6)->setData(1, Qt::DisplayRole, md5);

    /** O hash MD5 pode passar do comprimento do diálogo de informações.
     * Por isso, o ajuste precisa ser feito. */
    setSize(StatisticsView::HeaderSize);
}


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
    return { tr("Format"),        // 0
             tr("Codec ID"),       // 1
             tr("Bit Rate"),        // 2
             tr("Aspect Ratio"),     // 3
             tr("Size"),              // 4
             tr("Frame Rate"),         // 5
             tr("Frames"),              // 6
             tr("Bit Depth"),            // 7
             tr("Chroma Subsampling") }; // 8
}


/** Especificações para informações de áudio */
QStringList StatisticsView::getAudioInfoKeys() {
    return { tr("Format"),      // 0
             tr("Codec ID"),     // 1
             tr("Bit Rate"),      // 2
             tr("Sample Rate"),   // 3
             tr("Sample Format"), // 4
             tr("Channels"),      // 5
             tr("Frame Rate"),    // 6
             tr("Frame Size"),    // 7
             tr("Frames"),        // 8
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
                                   const QStringList &valuesAudio, const QStringList &valuesDual,
                                   const QStringList &metadataval, const QString &format) {
    Q_EMIT emitFormat(format, statistics.duration.msecsSinceStartOfDay());

    QList<QStringList> v{values, valuesVideo, valuesAudio, valuesDual, metadataval};
    int j = 0;
    foreach(QList<QTreeWidgetItem*> list, itemTab) {
        int i = 0;
        foreach(QTreeWidgetItem *it, list) {
            if (i == v.at(j).count()) break; /** Evita erros de segmentação */
            if (it->data(1, Qt::DisplayRole) != v.at(j).at(i)) it->setData(1, Qt::DisplayRole, v.at(j).at(i));
            ++i;
        }
        j++;
    }

    baseItems.at(6)->setData(1, Qt::DisplayRole, tr("Calculating..."));
    worker->setFile(url);
    worker->requestWork();

    settaginfos();
    visibility();
    setSize(StatisticsView::NormalSize);
}


/** Resetando as informações de estatísticas */
void StatisticsView::resetValues() {
    if (this->isVisible()) return;

    foreach(QList<QTreeWidgetItem*> list, itemTab)
    foreach(QTreeWidgetItem* it, list) it->setData(1, Qt::DisplayRole, "");
    for (int i = 1; i < 5; i++) tab->setTabVisible(i, false);

    statistics = QtAV::Statistics();
    ratio->setVisible(false);
    screen->setVisible(false);
    setSize(StatisticsView::InitialSize);
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


/** Iniciar busca por estatísticas */
void StatisticsView::onStatistics() {
    QtAV::Statistics dual{};
    statistics = st->statistics();
    if (st->audioStreamCount() > 1) {
        st->setAudioStream(1);
        dual = st->statistics();
    }
    st->stop();

    /** Pegando as informações */
    if (thread->isRunning()) thread->quit();
    if (thread2->isRunning()) thread2->quit();
    statisticsworker->setFile(url, statistics, dual);
    statisticsworker->requestWork();
}


/** Verificar o que precisa ser oculto */
void StatisticsView::visibility() {

    /** Redefinindo visualização */
    for (int i = 0; i < 5; i++) tab->setTabVisible(i, false);
    for (int i = 0; i < 5; i++) tab->setTabVisible(i, true);
    foreach(TreeView *t, itemView)
        for (int i = 0; i < t->topLevelItemCount(); ++i) t->topLevelItem(i)->setHidden(false);

    /** Verificando status de vídeo, áudio e metadata que precisam ser ocultos */
    int j = 1;
    foreach(QList<QTreeWidgetItem*> list, itemTab) {
        if (list == baseItems) continue;
        bool visibility{false};
        int i = 0;
        foreach(QTreeWidgetItem* it, list) {
            if (it->data(1, Qt::DisplayRole).toString().isEmpty()) itemView.at(j)->topLevelItem(i)->setHidden(true);
            else visibility = true;
            i++;
        }
        if (!visibility) tab->setTabVisible(j, false);
        j++;
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
    screen->setVisible(false);

    if (statistics.video_only.width == 0 || statistics.video_only.height == 0 || statistics.url.isEmpty()) return;
    int w = statistics.video_only.width;
    int h = statistics.video_only.height;
    QString size{QString::number((float)w / (float)h, 'f', 5)};

    if (QString::compare(size, "1.77778") == 0) Utils::changeIcon(ratio, "ratio169");
    else if (QString::compare(size, "1.33333") == 0) Utils::changeIcon(ratio, "ratio43");
    else ratio->setVisible(false);

    QList<QList<int>> itw{fuhdw, uhdpw, uhdw};
    QList<QList<int>> ith{fuhdh, uhdph, uhdh};
    QStringList scr{"screen8k", "screen5k", "screen4k"};

    int n = 0;
    foreach (QList<int> iw, itw) {
        for (int i : iw) if (i == w) for (int j : ith.at(n)) if (j == h) {
            Utils::changeIcon(screen, scr.at(n));
            screen->setVisible(true);
            return;
        }
        n++;
    }

    if (w == 2560 && h == 1440) {
        Utils::changeIcon(screen, "screenqhd");
        screen->setVisible(true);
    } else if (w == 1920 && h == 1080) {
        Utils::changeIcon(screen, "screenfhd");
        screen->setVisible(true);
    } else if (w == 1280 && h == 720) {
        Utils::changeIcon(screen, "screenhd");
        screen->setVisible(true);
    }
}


/** Buscando o maior comprimento para a janela */
void StatisticsView::setSize(TypeSize size) {
    if (size == StatisticsView::InitialSize) {
        /** Esse StretchLastSection precisa ser falso para o cálculo preciso */
        view1->header()->setStretchLastSection(false);
        int width = view1->ItemWith() + 40;
        view1->header()->setStretchLastSection(true);
        int height = view1->ItemHeight() * 7 + 110;

        qDebug("%s(%sStatisticsView%s)%s::%sAjustando o tamanho de infoview em %i x %i ...\033[0m",
               GRE, RED, GRE, RED, BLU, width, height);

        this->setMinimumSize(width, height);
        this->resize(width, height);
    } else {
        foreach(TreeView *it, itemView) it->header()->setStretchLastSection(false);

        /** Cálculo do comprimento */
        int width = 0;
        foreach(TreeView *it, itemView) if (it->ItemWith() > width) width = it->ItemWith();
        width += 40;

        foreach(TreeView *it, itemView) it->header()->setStretchLastSection(true);

        if (size == StatisticsView::HeaderSize) {
            qDebug("%s(%sStatisticsView%s)%s::%sAjustando a largura de infoview em %i ...\033[0m",
                   GRE, RED, GRE, RED, BLU, width);
            this->setMinimumWidth(width);
        } else {
            /** Cálculo da altura */
            int j = 0;
            int height = 0;
            foreach(TreeView *it, itemView) {
                int t = 0;
                for (int i = 0; i < it->topLevelItemCount(); i++)
                    if (!itemTab.at(j)[i]->data(1, Qt::DisplayRole).toString().isEmpty()) t++;
                if (t > height) height = t;
                j++;
            }

            if (height < 7) height = 7;
            height = view1->ItemHeight() * height + 110;

            qDebug("%s(%sStatisticsView%s)%s::%sAjustando o tamanho de infoview em %i x %i ...\033[0m",
                   GRE, RED, GRE, RED, BLU, width, height);

            this->setMinimumSize(width, height);
            this->resize(width, height);
        }
    }
}


/** Função para alterar o ícone do botão */
void StatisticsView::changeIcons() {
    Utils::changeIcon(closebtn, "apply");
    settaginfos();
}


/** vai cair fora **/
void StatisticsView::setRightDB(float value) {
    if (statistics.url.isEmpty()) return;
    vuright = QString::number(value);
    audioItems[10]->setData(1, Qt::DisplayRole, QString::fromLatin1("( %1 dB ) ( %2 dB )").arg(vuleft, vuright));
}
void StatisticsView::setLeftDB(float value) {
    if (statistics.url.isEmpty()) return;
    vuleft = QString::number(value);
    audioItems[10]->setData(1, Qt::DisplayRole, QString::fromLatin1("( %1 dB ) ( %2 dB )").arg(vuleft, vuright));
}
/*******************/


/** Função para setar um arquivo para buscar as informações */
void StatisticsView::setFile(const QString &file) {
    /** Não precisa gastar recurso se a url é a mesma */
    if (QString::compare(file, url) == 0 && !statistics.url.isEmpty()) return;

    qDebug("%s(%sStatisticsView%s)%s::%sSetando informações para %s ...\033[0m", GRE, RED, GRE, RED, UPD,
           STR(QString(file).remove(QRegExp("\\/.+\\/"))));

    resetValues();
    url = file;
    st->play(file);
}


/** Função ativa pelo showEvent do dialog */
void StatisticsView::showDialog() {
    if (statistics.url.isEmpty()) {
        if (thread->isRunning()) worker->noEmit(true);
        baseItems.at(6)->setData(1, Qt::DisplayRole, "");
        setSize(StatisticsView::InitialSize);
    }
}


/**********************************************************************************************************************/


/** Ações ao finalizar o diálogo */
void StatisticsView::hideEvent(QHideEvent *event) {
    resetValues();
    QDialog::hideEvent(event);
}
