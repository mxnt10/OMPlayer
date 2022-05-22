#include <QCryptographicHash>
#include <QDataStream>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsOpacityEffect>
#include <QLayout>
#include <QFrame>
#include <QMoveEvent>
#include <QSpacerItem>
#include <QStandardPaths>

#include <filesystem>

#include "Button.h"
#include "Defines.h"
#include "ListView.h"
#include "PlayList.h"
#include "PlayListModel.h"
#include "Utils.h"

using namespace Qt;
using QStandardPaths::MoviesLocation;
using QSizePolicy::Expanding;
using std::filesystem::exists;


/**********************************************************************************************************************/


/** Construtor do painel da playlist */
PlayList::PlayList(QWidget *parent) : QWidget(parent) {
    this->setMouseTracking(true);
    isshow = false;
    maxRows = -1;
    model = new PlayListModel(this);


    /** Efeito semitransparente para o fundo da playlist */
    auto *effect = new QGraphicsOpacityEffect();
    effect->setOpacity(OPACY);


    /** Lista para visualização da playlist */
    listView = new ListView();
    listView->setModel(model);
    connect(listView, SIGNAL(doubleClicked(QModelIndex)), SLOT(onAboutToPlay(QModelIndex)));
    connect(listView, SIGNAL(clicked(QModelIndex)), SLOT(onSelect(QModelIndex)));
    connect(listView, SIGNAL(emitEnter()), SLOT(noHide()));


    /** Botões para o painel da playlist */
    addBtn = new Button("add", 32, "Add items");
    connect(addBtn, SIGNAL(clicked()), SLOT(addItems()));

    removeBtn = new Button("remove", 32, "Remove items");
    connect(removeBtn, SIGNAL(clicked()), SLOT(removeSelectedItems()));

    clearBtn = new Button("clean", 32, "Clear playlist");
    connect(clearBtn, SIGNAL(clicked()), SLOT(clearItems()));


    /** Plano de fundo da playlist */
    auto *bgpls = new QWidget();
    bgpls->setGraphicsEffect(effect);
    bgpls->setStyleSheet(Utils::setStyle("widget"));


    /** Organização dos botões */
    auto *hbl = new QHBoxLayout();
    hbl->addWidget(addBtn);
    hbl->addWidget(removeBtn);
    hbl->addStretch(1);
    hbl->addWidget(clearBtn);


    /** Gerando uma linha */
    auto line = new QFrame();
    line->setFixedHeight(2);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background: #cccccc");


    /** Layout da playlist */
    wpls = new QWidget();
    auto *vbl = new QVBoxLayout(wpls);
    vbl->setMargin(10);
    vbl->addSpacing(3);
    vbl->addLayout(hbl);
    vbl->addSpacing(3);
    vbl->addWidget(line);
    vbl->addSpacing(5);
    vbl->addWidget(listView);


    /** Layout do painel da playlist */
    auto *gbl = new QGridLayout();
    gbl->setMargin(10);
    gbl->addItem(new QSpacerItem(0, 0, Expanding, Expanding), 0, 0, 1, 2);
    gbl->addWidget(bgpls, 0, 1, 1, 3);
    gbl->addWidget(wpls, 0, 1, 1, 3);
    gbl->setAlignment(AlignRight);
    wpls->setVisible(false);
    setLayout(gbl);
}


/** Destrutor */
PlayList::~PlayList() = default;


/**********************************************************************************************************************/


/** Usado para setar a localização do arquivo da playlist para carregar e salvar a playlist */
void PlayList::setSaveFile(const QString &file) {
    mfile = file;
}


/** Função para carregar a playlist ao abrir */
void PlayList::load(bool second) {
    QCryptographicHash hash(QCryptographicHash::Md5);
    QFile f(mfile);
    if (!f.exists()) save();
    if (!f.open(QIODevice::ReadOnly)) return;

    /** Capturando o hash */
    char buf[2048];
    while (f.read(buf, 2048) > 0) {
        hash.addData(buf, 2048);
    }
    f.close();
    actsum = hash.result().toHex();
    qDebug("%s(%sDEBUG%s):%s Capturando MD5 Hash %s ...\033[0m", GRE, RED, GRE, BLU, actsum.toStdString().c_str());

    if (QString::compare(sum, actsum, CaseInsensitive)) {
        qDebug("%s(%sDEBUG%s):%s Carregando a playlist ...\033[0m", GRE, RED, GRE, ORA);
        if (second)
            model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());
        f.open(QIODevice::ReadOnly);
        QDataStream ds(&f);
        QList<PlayListItem> list;
        ds >> list;
        int add = 0;
        for (const auto & i : list) {
            if (exists(i.url().toStdString())) {
                insertItemAt(i, add);
                add++;
            }
        }
        f.close();
        sum = actsum;
    } else {
        qDebug("%s(%sDEBUG%s):%s MD5 Hash Coincide ...\033[0m", GRE, RED, GRE, BLU);
    }
    save();
}


/** Função para salvar a playlist ao fechar */
void PlayList::save() {
    qDebug("%s(%sDEBUG%s):%s Salvando a playlist ...\033[0m", GRE, RED, GRE, ORA);
    QCryptographicHash hash(QCryptographicHash::Md5);
    QFile f(mfile);
    if (!f.open(QIODevice::WriteOnly)) return;
    QDataStream ds(&f);
    ds << model->items();
    f.close();

    /** Capturando o hash */
    f.open(QIODevice::ReadOnly);
    char buf[2048];
    while (f.read(buf, 2048) > 0) {
        hash.addData(buf, 2048);
    }
    f.close();
    actsum = hash.result().toHex();
    sum = actsum;
}


/** Função para adicionar itens a playlist */
void PlayList::addItems(const QStringList &parms) {
    bool select = false;
    QString isplay;
    QStringList files;

    /** Hack para o mouse não ocultar no diálogo para abrir arquivos */
    for (int i = 0; i < 1000; i++)
        Utils::arrowMouse();

    if (parms.isEmpty())
        files = \
        QFileDialog::getOpenFileNames(
            nullptr, tr("Select multimedia files"),
            QStandardPaths::standardLocations(MoviesLocation).value(0, QDir::homePath()),
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
    else
        files = parms;

    int t = model->rowCount(QModelIndex());

    if (files.isEmpty()) return;
    for (int i = 0; i < files.size(); ++i) {
        const QString& file = files.at(i);
        if (!QFileInfo(file).isFile())
            continue;
        insert(file, i + t);
        if (!select) {
            isplay = file;
            select = true;
        }
    }
    save();
    emit firstPlay(isplay);
}


/** Adiciona os itens para salvar na playlist */
void PlayList::insert(const QString &url, int row, qint64 duration, const QString &format) {
    if (duration == 0)
        qDebug("%s(%sDEBUG%s):%s Adicionando %s ...\033[0m", GRE, RED, GRE, ADD, url.toStdString().c_str());
    else
        qDebug("%s(%sDEBUG%s):%s Atualizando %s ...\033[0m", GRE, RED, GRE, UPD, url.toStdString().c_str());
    PlayListItem item;
    item.setUrl(url);
    item.setDuration(duration);
    item.setFormat(format);

    QString title = url;
    if (!url.contains(QLatin1String("://")) || url.startsWith(QLatin1String("file://")))
        title = QFileInfo(url).fileName();
    item.setTitle(title);
    insertItemAt(item, row);

    /** Se a duração é diferente de zero, é porque algum item foi atualizado */
    if (duration != 0)
        listView->setCurrentIndex(model->index(row));
}


/** Adiciona os itens para serem visualizados na playlist */
void PlayList::insertItemAt(const PlayListItem &item, int row) {
    if (maxRows > 0 && model->rowCount(QModelIndex()) >= maxRows)
        model->removeRows(maxRows, model->rowCount(QModelIndex()) - maxRows + 1, QModelIndex());
    int i = model->items().indexOf(item, row + 1);
    if (i > 0)
        model->removeRow(i);
    if (!model->insertRow(row))
        return;
    if (row > 0) {
        i = model->items().lastIndexOf(item, row - 1);
        if (i >= 0)
            model->removeRow(i);
    }
    setItemAt(item, row);
}


/** Localiza um item para exibir na playlist */
void PlayList::setItemAt(const PlayListItem &item, int row) {
    model->setData(model->index(row), QVariant::fromValue(item), DisplayRole);
}


/** Retorna o número do item que está atualmente em execução */
int PlayList::selectItems() {
    if (listView->currentIndex().row() == -1)
        return 0;
    return listView->currentIndex().row();
}


/** Função usado para selecionar um item da playlist */
void PlayList::selectCurrent(int indx) {
    listView->clearSelection();
    listView->setCurrentIndex(model->index(indx));
}


/** Limpar item selecionado da playlist */
void PlayList::selectClean() {
    listView->setCurrentIndex(model->index(-1));
    listView->clearSelection();
}


/** Retorna uma url de um arquivo multimídia usando um valor inteiro especificado */
QString PlayList::getItems(int s) {
    QModelIndex index = model->index(s);
    auto pli = qvariant_cast<PlayListItem>(index.data(DisplayRole));
    return pli.url();
}


/** Função para retornar o número total de itens da playlist */
int PlayList::setListSize() {
    return model->rowCount(QModelIndex());
}


/** Função para remover apenas os itens selecionados */
void PlayList::removeSelectedItems(bool update) {
    QItemSelectionModel *selection = listView->selectionModel();
    if (!selection->hasSelection())
        return;
    QModelIndexList s = selection->selectedIndexes();
    for (int i = s.size() - 1; i >= 0; --i) {
        if (!update)
            emit emitremove(s.at(i).row());
        model->removeRow(s.at(i).row());
    }
    save();
}


/** Limpando os itens da playlist */
void PlayList::clearItems() {
    model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());
    emit emitstop();
    save();
}


/** Setando a duração do item atual */
qint64 PlayList::setDuration() {
    QModelIndex index = listView->currentIndex();
    auto pli = qvariant_cast<PlayListItem>(index.data(DisplayRole));
    return pli.duration();
}


/** Função para emitir o intem selecionado na playlist para execução com um duplo clique */
void PlayList::onAboutToPlay(const QModelIndex &index) {
    emit aboutToPlay(index.data(DisplayRole).value<PlayListItem>().url());
}


/** Função para emitir o intem selecionado na playlist */
void PlayList::onSelect(const QModelIndex &index) {
    emit selected(int(index.row()));
}


/** Assistente para emissão de sinal */
void PlayList::noHide() {
    emit emitnohide();
}


/**********************************************************************************************************************/


/** Mapeador de eventos que está servindo para ocultar e desocultar a playlist */
void PlayList::mouseMoveEvent(QMouseEvent *event) {
    if (event->x() > (this->width() - 300) && event->y() < this->height() - 5 && !isshow) {
        qDebug("%s(%sDEBUG%s):%s Mouse posicionado na playlist ...\033[0m", GRE, RED, GRE, VIO);
        emit emitnohide();
        wpls->setVisible(true);
        isshow = true;
    } else if ((event->x() < (this->width() - 300) || event->y() > this->height() - 5) && isshow) {
        emit emithide();
        wpls->setVisible(false);
        isshow = false;
    }
    QWidget::mouseMoveEvent(event);
}


/** Emissão que serve como assistência para ocultar os controles */
bool PlayList::event(QEvent *event) {
    if (int(event->type()) == 110 && !isshow)
        emit emithiden();
    return QWidget::event(event);
}
