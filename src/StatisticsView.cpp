#include <QDebug>
#include <QFileInfo>
#include <QLayout>
#include <QTimerEvent>

#include "StatisticsView.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Interface principal para visualizar as informações de mídia */
StatisticsView::StatisticsView(QWidget *parent) : QDialog(parent) {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setModal(true);
    initItems(&baseItems, getBaseInfoKeys());
    initItems(&videoItems, getVideoInfoKeys());
    initItems(&audioItems, getAudioInfoKeys());


    /** Criando visualizador para as informações */
    view1 = new TreeView(baseItems);
    view2 = new TreeView(videoItems);
    view3 = new TreeView(audioItems);
    CTIME = baseItems[5];
    FPS = videoItems[6];


    /** Botão para fechar a janela */
    auto *closebtn = new Button("apply", 32, tr("Close"));
    connect(closebtn, SIGNAL(pressed()), SLOT(onClose()));


    /** Layout para as abas */
    auto *infogeral = new QWidget();
    auto *geral= new QVBoxLayout(infogeral);
    geral->addWidget(view1);
    auto *infovideo = new QWidget();
    auto *video = new QGridLayout(infovideo);
    video->addWidget(view2);
    auto *infoaudio = new QWidget();
    auto *audio = new QGridLayout(infoaudio);
    audio->addWidget(view3);


    /** Organização por abas */
    tab = new QTabWidget();
    tab->addTab(infogeral, tr("General"));
    tab->addTab(infovideo, tr("Video"));
    tab->addTab(infoaudio, tr("Audio"));
    tab->setStyleSheet(Utils::setStyle("tab"));


    /** Layout para organização das configurações */
    auto *fore = new QGridLayout();
    fore->setMargin(10);
    fore->addWidget(tab, 0, 0);
    fore->addWidget(closebtn, 1, 0, RIGHT);


    /** Fundo do diálogo de informações */
    auto *found = new QWidget();
    found->setStyleSheet(Utils::setStyle("widget"));


    /** Layout principal */
    auto *layout = new QGridLayout();
    layout->setMargin(0);
    layout->addWidget(found, 0, 0);
    layout->addLayout(fore, 0, 0);
    setLayout(layout);
}


/** Destrutor */
StatisticsView::~StatisticsView() = default;


/**********************************************************************************************************************/


/** Emissão para fechar a janela */
void StatisticsView::onClose() {
    qDebug("%s(%sDEBUG%s):%s Fechando o diálogo de configurações ...\033[0m", GRE, RED, GRE, CYA);
    emit emitclose();
    this->close();
}


/** Especificações das informações básicas */
QStringList StatisticsView::getBaseInfoKeys() {
    return QStringList()
        << tr("File Path")
        << tr("File Name")
        << tr("File Size")
        << tr("Format")
        << tr("Bit Rate")
        << tr("Duration");
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
        << tr("Channel Layout")
        << tr("Frames")
        << tr("Frame Size")
        << tr("Frame Rate");
}


/** Informações básicas de mídia */
QVariantList StatisticsView::getBaseInfoValues(const Statistics& s) {
    QString rate, duration;
    if (!s.url.isEmpty()) {
        rate = QString::number(s.bit_rate/1000).append(QString::fromLatin1(" Kb/s"));
        duration = QString::fromLatin1("%1 / %2").arg(ctime).arg(s.duration.toString(QString::fromLatin1("HH:mm:ss")));
    }

    return QVariantList()
        << QString(s.url).remove(QRegExp("\\/(?:.(?!\\/))+$"))
        << QString(s.url).remove(QRegExp("\\/.+\\/"))
        << fsize
        << s.format
        << rate
        << duration;
}


/** Informações de vídeo */
QVariantList StatisticsView::getVideoInfoValues(const Statistics& s) {
    QString ratio = QString::number(((double)s.video_only.width / s.video_only.height));
    QString sizev = QString::fromLatin1("%1x%2").arg(s.video_only.width).arg(s.video_only.height);
    QString sizec = QString::fromLatin1("%1x%2").arg(s.video_only.coded_width).arg(s.video_only.coded_height);

    return QVariantList()
        << QString::fromLatin1("%1 (%2)").arg(s.video.codec).arg(s.video.codec_long)
        << QString::fromLatin1("%1 (%2)").arg(s.video.decoder).arg(s.video.decoder_detail)
        << QString::number(s.video.bit_rate/1000).append(QString::fromLatin1(" Kb/s"))
        << ratio
        << QString::fromLatin1("%1 (Codec: %2)").arg(sizev).arg(sizec)
        << s.video.frames
        << QString::fromLatin1("%1 / %2").arg(QString::number(
                s.video.frame_rate, 'f', 2)).arg(QString::number(s.video.frame_rate, 'f', 2))
        << s.video_only.pix_fmt
        << s.video_only.gop_size;
}


/** Informações de áudio */
QVariantList StatisticsView::getAudioInfoValues(const Statistics& s) {
    return QVariantList()
        << QString::fromLatin1("%1 (%2)").arg(s.audio.codec).arg(s.audio.codec_long)
        << QString::fromLatin1("%1 (%2)").arg(s.audio.decoder).arg(s.audio.decoder_detail)
        << QString::number(s.audio.bit_rate/1000).append(QString::fromLatin1(" Kb/s"))
        << QString::number(s.audio_only.sample_rate).append(QString::fromLatin1(" Hz"))
        << s.audio_only.sample_fmt
        << s.audio_only.channels
        << s.audio_only.channel_layout
        << s.audio.frames
        << s.audio_only.frame_size
        << s.audio.frame_rate;
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
void StatisticsView::setStatistics(const Statistics& s) {
    qDebug("%s(%sDEBUG%s):%s Atualizando informações para %s ...\033[0m", GRE, RED, GRE, UPD,
           qUtf8Printable(QString(s.url).remove(QRegExp("\\/.+\\/"))));

    int i = 0;
    ctime = "00:00:00";
    statistics = s;
    QString byte{"B"};
    QFileInfo file{s.url};
    auto size = (double)file.size();

    /** Definindo o tamanho do arquivo de mídia */
    while (size > 1024) {
        size = size / 1024;
        i++;
    }

    if (i == 1) byte = "KiB";
    else if (i == 2) byte = "MiB";
    else if (i == 3) byte = "GiB";
    fsize = QString::fromLatin1("%1 %2 (%3)").arg(QString::number(size, 'f', 2)).arg(byte).arg(file.size());

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

    /** Buscando o maior comprimento para a janela */
    visibility();
    int csize = view1->size();
    if (csize < view2->size()) csize = view2->size();
    if (csize < view3->size()) csize = view3->size();
    csize = csize + 40;
    qDebug("%s(%sDEBUG%s):%s Ajustando comprimento de infoview em %i ...\033[0m", GRE, RED, GRE, BLU, csize);

    this->setMinimumSize(csize, 350);
    this->resize(csize, this->height());
}


/** Função para setar a duração atual da mídia reproduzida */
void StatisticsView::setCurrentTime(int current) {
    ctime = QTime(0, 0, 0).addMSecs(current).toString(QString::fromLatin1("HH:mm:ss"));
}


/** Verificar o que precisa ser oculto */
void StatisticsView::visibility(){
    /** Redefinindo visualização */
    for (int i = 0; i < 3; i++) tab->setTabVisible(i, false);
    for (int i = 0; i < 3; i++) tab->setTabVisible(i, true);
    for (int i = 0; i < view2->topLevelItemCount(); ++i) view2->topLevelItem(i)->setHidden(false);
    for (int i = 0; i < view3->topLevelItemCount(); ++i) view3->topLevelItem(i)->setHidden(false);

    /** Verificando status de vídeo e áudio e o que precisa ser oculto */
    if (!statistics.video.available) tab->setTabVisible(1, false);
    else {
        if (statistics.video.bit_rate == 0) view2->topLevelItem(2)->setHidden(true);
        if (statistics.video.frames == 0) view2->topLevelItem(5)->setHidden(true);
        if (statistics.video.frame_rate == 0) view2->topLevelItem(6)->setHidden(true);
    }
    if (!statistics.audio.available) tab->setTabVisible(2, false);
    else {
        if (statistics.audio.bit_rate == 0) view3->topLevelItem(2)->setHidden(true);
        if (statistics.audio.frames == 0) view3->topLevelItem(7)->setHidden(true);
        if (statistics.audio_only.frame_size == 0) view3->topLevelItem(8)->setHidden(true);
        if (statistics.audio.frame_rate == 0) view3->topLevelItem(9)->setHidden(true);
    }
}


/**********************************************************************************************************************/


/** Evento para finalizar o temporizador */
void StatisticsView::hideEvent(QHideEvent *event) {
    QDialog::hideEvent(event);
    killTimer(timer);
}


/** Evento para iniciar o temporizador */
void StatisticsView::showEvent(QShowEvent *event) {
    visibility();
    QDialog::showEvent(event);
    timer = startTimer(1000);
}


/** Evento de temporização */
void StatisticsView::timerEvent(QTimerEvent *event) {
    if (event->timerId() != timer) return;

    if (FPS && statistics.video.frame_rate != 0)
        FPS->setData(1, Qt::DisplayRole,
                     QString::fromLatin1("%1 / %2").arg(QString::number(statistics.video.frame_rate, 'f', 2)).arg(
                             QString::number(statistics.video_only.currentDisplayFPS(),'f', 2)));

    if (CTIME && !statistics.url.isEmpty())
        CTIME->setData(1, Qt::DisplayRole, QString::fromLatin1("%1 / %2").arg(ctime).arg(
                statistics.duration.toString(QString::fromLatin1("HH:mm:ss"))));
}
