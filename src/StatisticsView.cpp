#include <QCloseEvent>
#include <QLayout>
#include <QHeaderView>
#include <QTimer>
#include <QTimerEvent>
#include <QAbstractScrollArea>

#include "StatisticsView.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Interface principal para visualizar as informações de mídia */
StatisticsView::StatisticsView(QWidget *parent) : QDialog(parent) {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet(Utils::setStyle("infoview"));
    setModal(true);
    initBaseItems(&baseItems);

    /** Criando visualizador para as informações */
    view = new QTreeWidget();
    view->setHeaderHidden(true);
    view->setColumnCount(2);
    view->addTopLevelItems(baseItems);

    /** Adicionando informações do vídeo */
    itemv = createNodeWithItems(view, tr("Video"), getVideoInfoKeys(), &videoItems);
    FPS = itemv->child(6);
    view->addTopLevelItem(itemv);

    /** Adicionando informações de áudio */
    itema = createNodeWithItems(view, tr("Audio"), getAudioInfoKeys(), &audioItems);
    view->addTopLevelItem(itema);

    /** Ajustes no redirecionamento das colunas */
    view->header()->setStretchLastSection(false);
    view->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    /** Botão para fechar a janela */
    auto *closebtn = new Button("apply", 32, tr("Close"));
    connect(closebtn, SIGNAL(pressed()), SLOT(onClose()));

    /** Ajustes nos botões */
    auto *btn = new QHBoxLayout();
    btn->setContentsMargins(0, 0, 2, 0);
    btn->addWidget(closebtn);

    /** Ajustes no treeview */
    auto *tree = new QGridLayout();
    tree->setContentsMargins(5, 4, 8, 10);
    tree->addWidget(view, 0, 0);
    tree->addLayout(btn, 1, 0, RIGHT);

    /** Layout do diálogo de informações */
    auto *fd = new QWidget();
    auto *vl = new QGridLayout();
    vl->setMargin(0);
    vl->addWidget(fd, 0, 0);
    vl->addLayout(tree, 0, 0);
    setLayout(vl);
}


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
        << tr("Format")
        << tr("Bit rate")
        << tr("Duration");
}


/** Informações básicas de mídia */
QVariantList StatisticsView::getBaseInfoValues(const Statistics& s) {
    QString rate;
    if (!s.url.isEmpty()) rate = QString::number(s.bit_rate/1000).append(QString::fromLatin1(" Kb/s"));
    return QVariantList()
        << QString(s.url).remove(QRegExp("\\/(?:.(?!\\/))+$"))
        << QString(s.url).remove(QRegExp("\\/.+\\/"))
        << s.format
        << rate
        << s.duration.toString(QString::fromLatin1("HH:mm:ss"));
}


/** Especificações gerais para vídeo e áudio */
QStringList StatisticsView::getCommonInfoKeys() {
    return QStringList()
        << tr("Codec")
        << tr("Decoder")
        << tr("Decoder Detail")
        << tr("Bit Rate")
        << tr("Frames")
        << tr("FPS"); //frame rate
}


/** Especificações para informações de vídeo */
QStringList StatisticsView::getVideoInfoKeys() {
    return getCommonInfoKeys()
        << tr("FPS Now") //current frame rate
        << tr("Pixel Format")
        << tr("Size")
        << tr("Coded Size")
        << tr("GOP Size");
}


/** Especificações para informações de áudio */
QStringList StatisticsView::getAudioInfoKeys() {
    return getCommonInfoKeys()
        << tr("Sample format")
        << tr("Sample rate")
        << tr("Channels")
        << tr("Channel layout")
        << tr("Frame size");
}


/** Informações de vídeo */
QList<QVariant> StatisticsView::getVideoInfoValues(const Statistics& s) {
    return QList<QVariant>()
        << QString::fromLatin1("%1 (%2)").arg(s.video.codec).arg(s.video.codec_long)
        << s.video.decoder
        << s.video.decoder_detail
        << QString::number(s.video.bit_rate/1000).append(QString::fromLatin1(" Kb/s"))
        << s.video.frames
        << s.video.frame_rate
        << s.video.frame_rate
        << s.video_only.pix_fmt
        << QString::fromLatin1("%1x%2").arg(s.video_only.width).arg(s.video_only.height)
        << QString::fromLatin1("%1x%2").arg(s.video_only.coded_width).arg(s.video_only.coded_height)
        << s.video_only.gop_size;
}


/** Informações de áudio */
QList<QVariant> StatisticsView::getAudioInfoValues(const Statistics& s) {
    return QList<QVariant>()
        << QString::fromLatin1("%1 (%2)").arg(s.audio.codec).arg(s.audio.codec_long)
        << s.audio.decoder
        << s.audio.decoder_detail
        << QString::number(s.audio.bit_rate/1000).append(QString::fromLatin1(" Kb/s"))
        << s.audio.frames
        << s.audio.frame_rate
        << s.audio_only.sample_fmt
        << QString::number(s.audio_only.sample_rate).append(QString::fromLatin1(" Hz"))
        << s.audio_only.channels
        << s.audio_only.channel_layout
        << s.audio_only.frame_size;
}


/** Função para coletar as informações básicas dos items */
void StatisticsView::initBaseItems(QList<QTreeWidgetItem *> *items) {
    QTreeWidgetItem *item;
    foreach(const QString& key, getBaseInfoKeys()) {
        item = new QTreeWidgetItem(0);
        item->setData(0, Qt::DisplayRole, key);
        items->append(item);
    }
}


/** Função usada para setar as estatísticas dos itens atuais */
void StatisticsView::setStatistics(const Statistics& s) {
    statistics = s;
    QVariantList v = getBaseInfoValues(s);
    int i = 0;

    foreach(QTreeWidgetItem* it, baseItems) {
        if (it->data(1, Qt::DisplayRole) != v.at(i)) it->setData(1, Qt::DisplayRole, v.at(i));
        ++i;
    }

    v = getVideoInfoValues(s);
    i = 0;

    foreach(QTreeWidgetItem* it, videoItems) {
        if (it->data(1, Qt::DisplayRole) != v.at(i)) it->setData(1, Qt::DisplayRole, v.at(i));
        ++i;
    }

    v = getAudioInfoValues(s);
    i = 0;

    foreach(QTreeWidgetItem* it, audioItems) {
        if (it->data(1, Qt::DisplayRole) != v.at(i)) it->setData(1, Qt::DisplayRole, v.at(i));
        ++i;
    }
}


/** Função para criar novos níveis para informações de vídeo e áudio */
QTreeWidgetItem* StatisticsView::createNodeWithItems(QTreeWidget *view, const QString &name,
                                                     const QStringList &itemNames,
                                                     QList<QTreeWidgetItem *> *items) {
    auto *nodeItem = new QTreeWidgetItem(view);
    nodeItem->setData(0, Qt::DisplayRole, name);
    QTreeWidgetItem *item;

    foreach(const QString& key, itemNames) {
        item = new QTreeWidgetItem(nodeItem);
        item->setData(0, Qt::DisplayRole, key);
        nodeItem->addChild(item);
        if (items) items->append(item);
    }

    nodeItem->setExpanded(true);
    return nodeItem;
}


/**********************************************************************************************************************/


/** Evento para finalizar o temporizador */
void StatisticsView::hideEvent(QHideEvent *event) {
    QDialog::hideEvent(event);
    killTimer(timer);
}


/** Evento para iniciar o temporizador */
void StatisticsView::showEvent(QShowEvent *event) {
    if (!statistics.video.available) itemv->setHidden(true);
    else itemv->setHidden(false);

    if (!statistics.audio.available) itema->setHidden(true);
    else itema->setHidden(false);

    if (statistics.url.isEmpty()) {
        this->setMinimumSize((view->header()->sectionSize(0) + view->header()->sectionSize(1) + 7), 180);
        this->resize((view->header()->sectionSize(0) + view->header()->sectionSize(1) + 7), 180);
    } else {
        this->setMinimumSize((view->header()->sectionSize(0) + view->header()->sectionSize(1) + 16), 360);
        this->resize((view->header()->sectionSize(0) + view->header()->sectionSize(1) + 16), this->height());
    }

    QDialog::showEvent(event);
    timer = startTimer(1000);
}


/** Evento de temporização */
void StatisticsView::timerEvent(QTimerEvent *event) {
    if (event->timerId() != timer) return;
    if (FPS) FPS->setData(1, Qt::DisplayRole, QString::number(statistics.video_only.currentDisplayFPS(), 'f', 2));
}


/** Ação ao fechar o diálogo */
void StatisticsView::closeEvent(QCloseEvent *event) {
    qDebug("%s(%sDEBUG%s):%s Finalizando as informações de mídia atual!\033[0m", GRE, RED, GRE, CYA);
    emit emitclose();
    event->accept();
}
