#include <QtAV>
#include <QScrollArea>
#include <QLabel>
#include <Utils>

#include "Decoder.h"


/**********************************************************************************************************************/


/** construtor */
Decoder::Decoder(QWidget *parent): QWidget(parent) {

    /** Widget para inserir os widgets gerados */
    auto *scrollAreaWidgetContents = new QWidget(this);
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollAreaWidgetContents);


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
    auto *vb = new QVBoxLayout();
    vb->setSpacing(0);
    vb->addWidget(new QLabel("Decoder options (Open new media is required)"));
    vb->addLayout(decLayout);


    /** Layout para ajustar os widgets gerados na */
    auto *vlsroll = new QVBoxLayout(scrollAreaWidgetContents);
    vlsroll->setSpacing(0);
    vlsroll->addLayout(vb);


    /** Layout geral */
    auto *vbs = new QVBoxLayout(this);
    vbs->addWidget(scrollArea);
}

/** Destrutor */
Decoder::~Decoder() = default;


/**********************************************************************************************************************/


/***/ //todo
void Decoder::videoDecoderEnableChanged() {}


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
    QStringList names;
    foreach (DecoderItemWidget *iw, decItems) {
        if (iw->isChecked()) names.append(iw->name());
    }
    qDebug() << STR(names.join(' '));
    return idsFromNames(names);
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
