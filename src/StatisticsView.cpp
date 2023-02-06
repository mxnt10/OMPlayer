#include <QHeaderView>
#include <QLayout>
#include <QScreen>
#include <Utils>

#include "StatisticsView.h"


/**********************************************************************************************************************/


/** Interface principal para visualizar as informações de mídia */
StatisticsView::StatisticsView(QWidget *parent) : Dialog(parent) {
    connect(this, &Dialog::emitclose, [this](){ Q_EMIT emitclose(); });

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


    /** Criando visualizador para as informações */
    view1 = new TreeView(baseItems);
    view2 = new TreeView(videoItems);
    view3 = new TreeView(audioItems);
    view4 = new TreeView(audioDual);
    view5 = new TreeView(metadata);


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
    baseItems[6]->setData(1, Qt::DisplayRole, md5);

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
    return { tr("Codec"),        // 0
             tr("Decoder"),       // 1
             tr("Bit Rate"),       // 2
             tr("Aspect Ratio"),    // 3
             tr("Size"),             // 4
             tr("Frames"),            // 5
             tr("Frame Rate"),         // 6
             tr("Bit Depth"),           // 7
             tr("Chroma Subsampling") }; // 8
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
                                   const QStringList &valuesAudio, const QStringList &valuesDual,
                                   const QStringList &metadataval,
                                   const QString &format, int duration) {
    Q_EMIT emitFormat(format, duration);

    QList<QList<QTreeWidgetItem*>> item{baseItems, videoItems, audioItems, audioDual, metadata};
    QList<QStringList> v{values, valuesVideo, valuesAudio, valuesDual, metadataval};
    int j = 0;
    foreach(QList<QTreeWidgetItem*> list, item) {
        int i = 0;
        foreach(QTreeWidgetItem *it, list) {
            if (i == v[j].count()) break; /** Evita erros de segmentação */
            if (it->data(1, Qt::DisplayRole) != v[j].at(i)) it->setData(1, Qt::DisplayRole, v[j].at(i));
            ++i;
        }
        j++;
    }

    baseItems[6]->setData(1, Qt::DisplayRole, tr("Calculating..."));
    worker->setFile(url);
    worker->requestWork();

    settaginfos();
    visibility();
    setSize(StatisticsView::NormalSize);
}


/** Resetando as informações de estatísticas */
void StatisticsView::resetValues() {
    if (this->isVisible()) return;

    QList<QList<QTreeWidgetItem*>> item{baseItems, videoItems, audioItems, audioDual, metadata};
    foreach(QList<QTreeWidgetItem*> list, item)
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


/** Função usada para setar as estatísticas dos itens atuais */
void StatisticsView::setStatistics(const QtAV::Statistics& s) {
    qDebug("%s(%sStatisticsView%s)%s::%sAtualizando informações para %s ...\033[0m", GRE, RED, GRE, RED, UPD,
           STR(QString(s.url).remove(QRegExp("\\/.+\\/"))));

//    qDebug() << s.metadata.keys();

    /** Redefinindo informações */
    resetValues();
    if (!s.url.isEmpty()) {
        statistics = s;
        url = s.url;
    }

    if (thread->isRunning()) thread->quit();
    if (thread2->isRunning()) thread2->quit();
    statisticsworker->setFile(url, s);
    statisticsworker->requestWork();
}


/** Função para setar a duração atual da mídia reproduzida */
void StatisticsView::setCurrentTime(int current) {
    if (baseItems[0]->data(1, Qt::DisplayRole).toString().isEmpty() || statistics.url.isEmpty()) return;

    /** Resetando tempo de execução ao reproduzir a próxima mídia */
    if (QString::compare(currentStatistics.url, url) != 0) {
        baseItems[5]->setData(1, Qt::DisplayRole, statistics.duration.toString(QString::fromLatin1("HH:mm:ss")));
        return;
    }

    baseItems[5]->setData(1, Qt::DisplayRole, QString::fromLatin1("%1 / %2").arg(
            QTime(0, 0, 0).addMSecs(current).toString(QString::fromLatin1("HH:mm:ss")),
            statistics.duration.toString(QString::fromLatin1("HH:mm:ss"))));
}


/** Verificar o que precisa ser oculto */
void StatisticsView::visibility(){
    QList<TreeView*> view{view2, view3, view4, view5};
    QList<QList<QTreeWidgetItem*>> item{videoItems, audioItems, audioDual, metadata};

    /** Redefinindo visualização */
    for (int i = 0; i < 5; i++) tab->setTabVisible(i, false);
    for (int i = 0; i < 5; i++) tab->setTabVisible(i, true);
    foreach(TreeView *t, view) for (int i = 0; i < t->topLevelItemCount(); ++i) t->topLevelItem(i)->setHidden(false);

    /** Verificando status de vídeo, áudio e metadata que precisam ser ocultos */
    int j = 1;
    foreach(QList<QTreeWidgetItem*> list, item) {
        bool visibility{false};
        int i = 0;
        foreach(QTreeWidgetItem* it, list) {
            if (it->data(1, Qt::DisplayRole).toString().isEmpty()) view[j - 1]->topLevelItem(i)->setHidden(true);
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

    if (videoItems[3]->data(1, Qt::DisplayRole).toString().split(' ')[0] == "16:9")
        Utils::changeIcon(ratio, "ratio169");
    else if (videoItems[3]->data(1, Qt::DisplayRole).toString().split(' ')[0] == "4:3")
        Utils::changeIcon(ratio, "ratio43");
    else ratio->setVisible(false);

    QStringList size = videoItems[4]->data(1, Qt::DisplayRole).toString().split(' ')[0].split('x');
    if (size[0].toInt() == 0) return;
    int w = size[0].toInt();
    int h = size[1].toInt();

    QList<QList<int>> itw{fuhdw, uhdpw, uhdw};
    QList<QList<int>> ith{fuhdh, uhdph, uhdh};
    QStringList scr{"screen8k", "screen5k", "screen4k"};

    int n = 0;
    foreach (QList<int> iw, itw) {
        for (int i : iw) if (i == w) for (int j : ith[n]) if (j == h) {
            Utils::changeIcon(screen, scr[n]);
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
        QList<TreeView *> view = {view1, view2, view3, view4, view5};
        QList<QList<QTreeWidgetItem *>> item{baseItems, videoItems, audioItems, audioDual, metadata};
        foreach(TreeView *it, view) it->header()->setStretchLastSection(false);

        /** Cálculo do comprimento */
        int width = 0;
        foreach(TreeView *it, view) if (it->ItemWith() > width) width = it->ItemWith();
        width += 40;

        foreach(TreeView *it, view) it->header()->setStretchLastSection(true);

        if (size == StatisticsView::HeaderSize) {
            qDebug("%s(%sStatisticsView%s)%s::%sAjustando a largura de infoview em %i ...\033[0m",
                   GRE, RED, GRE, RED, BLU, width);
            this->setMinimumWidth(width);
        } else {
            /** Cálculo da altura */
            int j = 0;
            int height = 0;
            foreach(TreeView *it, view) {
                int t = 0;
                for (int i = 0; i < it->topLevelItemCount(); i++)
                    if (!item.at(j)[i]->data(1, Qt::DisplayRole).toString().isEmpty()) t++;
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

    /** Ajuste do centro */
    this->move(0, 0);
    this->move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center());
}


/** Função para alterar o ícone do botão */
void StatisticsView::changeIcons() {
    Utils::changeIcon(closebtn, "apply");
    settaginfos();
}


/** Setando informações do dB direito */
void StatisticsView::setRightDB(float value) {
    if (statistics.url.isEmpty()) return;
    vuright = QString::number(value);
    audioItems[10]->setData(1, Qt::DisplayRole, QString::fromLatin1("( %1 dB ) ( %2 dB )").arg(vuleft, vuright));
}


/** Setando informações do dB esquerdo */
void StatisticsView::setLeftDB(float value) {
    if (statistics.url.isEmpty()) return;
    vuleft = QString::number(value);
    audioItems[10]->setData(1, Qt::DisplayRole, QString::fromLatin1("( %1 dB ) ( %2 dB )").arg(vuleft, vuright));
}


/** Função para setar um arquivo para buscar as informações */
void StatisticsView::setFile(const QString &file) {
    /** Não precisa gastar recurso se a url é a mesma */
    if (QString::compare(file, url) == 0 && !statistics.url.isEmpty()) return;
    url = file;
    if (QString::compare(currentStatistics.url, url) == 0) setStatistics(currentStatistics);
    else setStatistics();
}


/**********************************************************************************************************************/


/** Evento para finalizar o temporizador */
void StatisticsView::hideEvent(QHideEvent *event) {
    resetValues();
    QDialog::hideEvent(event);
}
