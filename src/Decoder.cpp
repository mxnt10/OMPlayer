#include <QtAV>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <Utils>

#include "Decoder.h"


/**********************************************************************************************************************/


/** construtor */
Decoder::Decoder(QWidget *parent): QWidget(parent) {

    /** Widget para inserir os widgets gerados */
    auto *scrollAreaWidgetContents = new QWidget(this);
    scrollAreaWidgetContents->setAttribute(Qt::WA_NoSystemBackground, true);
    scrollAreaWidgetContents->setAttribute(Qt::WA_TranslucentBackground, true);
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollAreaWidgetContents);
    scrollArea->setStyleSheet(Utils::setStyle("playlist"));


    /** Definindo opções de decodificação */
    QStringList vds{"cuda", "vaapi", "vda", "dxva", "cedarv", "ffmpeg"};
    QVector<QtAV::VideoDecoderId> vids = idsFromNames(vds);
    QVector<QtAV::VideoDecoderId> vds_all = QtAV::VideoDecoder::registered();
    QVector<QtAV::VideoDecoderId> all = vids;


    /** Validando itens */
    foreach (QtAV::VideoDecoderId vid, vds_all) {
        if (!vids.contains(vid)) all.push_back(vid);
    }


    /** Gerando os itens */
    decLayout = new QVBoxLayout();
    decLayout->setSpacing(0);
    foreach (QtAV::VideoDecoderId vid, all) {
        auto *vd = QtAV::VideoDecoder::create(vid);
        auto *iw = new DecoderItemWidget(scrollAreaWidgetContents);
        iw->buildUiFor(vd);
        iw->setName(vd->name());
        iw->setDescription(vd->description());
        iw->setChecked(vids.contains(vid));
        connect(iw, &DecoderItemWidget::enableChanged, this, &Decoder::videoDecoderEnableChanged);

        decItems.append(iw);
        decLayout->addWidget(iw);
        delete vd;
    }


    /** Layout dos widgets gerados */
    auto *label = new QLabel(nullptr, this);
    label->setText(QApplication::tr("Decoder options") + " (" + QApplication::tr("Open new media is required") + ")");
    auto *vb = new QVBoxLayout();
    vb->setSpacing(0);
    vb->addWidget(label);
    vb->addLayout(decLayout);


    /** Layout para ajustar os widgets gerados */
    auto *vlsroll = new QVBoxLayout(scrollAreaWidgetContents);
    vlsroll->setSpacing(0);
    vlsroll->addLayout(vb);


    /** Layout geral */
    auto *vbs = new QVBoxLayout(this);
    vbs->setMargin(1);
    vbs->addWidget(scrollArea);


    /** Desativando menu de contexto dos scroolbar */
    foreach(QObject *widget, qApp->allWidgets()) {
        auto *scrollBar = dynamic_cast<QScrollBar*>(widget);
        if(scrollBar) scrollBar->setContextMenuPolicy(Qt::NoContextMenu);
    }
}


/** Destrutor */
Decoder::~Decoder() = default;


/**********************************************************************************************************************/


/** Setando opções ao selecionar as opções de decodificação */
void Decoder::videoDecoderEnableChanged() {
    QStringList names;
    foreach (DecoderItemWidget *iw, decItems) {
        if (iw->isChecked()) names.append(iw->name());
    }
    qDebug("%s(%sDecoder%s)%s::%sDecodificações ativas (%s)\033[0m", GRE, RED, GRE, RED, HID, STR(names.join(' ')));
    optionDecoder = names;
}


/** Opções de decodificação de vídeo */
QVariantHash Decoder::videoDecoderOptions() const {
    QVariantHash options;
    foreach (DecoderItemWidget* diw, decItems) {
        options[diw->name()] = diw->getOptions();
    }
    return options;
}


/** Setando prioridade de execução de decodificadores */
QVector<QtAV::VideoDecoderId> Decoder::decoderPriorityNames() {
    if (optionDecoder.isEmpty()) videoDecoderEnableChanged();
    return idsFromNames(optionDecoder);
}


/** Buscando o id das opções de decodificação */
QVector<QtAV::VideoDecoderId> Decoder::idsFromNames(const QStringList& names) {
    QVector<QtAV::VideoDecoderId> decs;
    foreach (QString name, names) {
        if (name.isEmpty()) continue;
        QtAV::VideoDecoderId id = QtAV::VideoDecoder::id(name.toLatin1().constData());
        if (id == 0) continue;
        decs.append(id);
    }
    return decs;
}


/** Alterando os ícones dos botões do widgets de decodificação */
void Decoder::changeIcons() {
    foreach (DecoderItemWidget* diw, decItems) {
        diw->changeIcons();
    }
}
