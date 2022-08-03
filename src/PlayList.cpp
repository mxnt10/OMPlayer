#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QLayout>
#include <QMoveEvent>
#include <QStandardPaths>
#include <QTextCodec>

#include <filesystem>

#include "EventFilter.h"
#include "PlayList.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Construtor do painel da playlist */
PlayList::PlayList(QWidget *parent) : QWidget(parent) {
    this->setMouseTracking(true);
    model = new PlayListModel(this);
    delegate = new PlayListDelegate(this);
    mfile = QDir::homePath() + "/.config/OMPlayer/playlist.qds";


    /** Lista para visualização da playlist */
    auto *filter = new EventFilter(this, 3);
    listView = new QListView();
    listView->setModel(model);
    listView->setItemDelegate(delegate);
    listView->setSelectionMode(QAbstractItemView::ExtendedSelection); /** Uso com CTRL/SHIF */
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listView->setStyleSheet(Utils::setStyle("playlist"));
    listView->setFixedWidth(280);
    listView->installEventFilter(filter);
    connect(listView, SIGNAL(doubleClicked(QModelIndex)), SLOT(onAboutToPlay(QModelIndex)));
    connect(listView, SIGNAL(clicked(QModelIndex)), SLOT(onSelect(QModelIndex)));
    connect(filter, SIGNAL(emitEnter()), SLOT(enterList()));
    connect(filter, SIGNAL(emitLeave()), SLOT(leaveList()));


    /** Botões para o painel da playlist */
    addBtn = new Button("add", 32, tr("Add items"));
    connect(addBtn, SIGNAL(clicked()), SLOT(addItems()));

    removeBtn = new Button("remove", 32, tr("Remove items"));
    connect(removeBtn, SIGNAL(clicked()), SLOT(removeSelectedItems()));

    clearBtn = new Button("clean", 32, tr("Clear playlist"));
    connect(clearBtn, SIGNAL(clicked()), SLOT(clearItems()));


    /** Plano de fundo da playlist */
    auto *bgpls = new QWidget();
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
    gbl->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding), 0, 0, 1, 2);
    gbl->addWidget(bgpls, 0, 1, 1, 3);
    gbl->addWidget(wpls, 0, 1, 1, 3);
    gbl->setAlignment(RIGHT);
    wpls->setVisible(false);
    setLayout(gbl);
}


/** Destrutor */
PlayList::~PlayList() = default;


/**********************************************************************************************************************/


/** Função para carregar a playlist ao abrir */
void PlayList::load(bool second) {
    QFile f(mfile);
    if (!f.exists()) save();
    if (!f.open(QIODevice::ReadOnly)) return;

    actsum = Utils::setHash(mfile);
    qDebug("%s(%sDEBUG%s):%s Capturando MD5 Hash %s ...\033[0m", GRE, RED, GRE, BLU, qUtf8Printable(actsum));

    if (QString::compare(sum, actsum, Qt::CaseInsensitive)) {
        qDebug("%s(%sDEBUG%s):%s Carregando a playlist ...\033[0m", GRE, RED, GRE, ORA);
        if (second) model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());
        QDataStream ds(&f);
        QList<PlayListItem> list;
        ds >> list;
        int add = 0;
        for (const auto & i : list) {
            if (std::filesystem::exists(i.url().toStdString())) {
                hashlist.append(Utils::stringHash(i.url()));
                insertItemAt(i, add);
                add++;
            }
        }
        sum = actsum;
    } else
        qDebug("%s(%sDEBUG%s):%s MD5 Hash Coincide ...\033[0m", GRE, RED, GRE, BLU);
    f.close();
    save();
}


/** Função para salvar a playlist ao fechar */
void PlayList::save() {
    qDebug("%s(%sDEBUG%s):%s Salvando a playlist ...\033[0m", GRE, RED, GRE, ORA);
    QFile f(mfile);
    if (!f.open(QIODevice::WriteOnly)) return;
    QDataStream ds(&f);
    ds << model->items();
    f.close();
    actsum = Utils::setHash(mfile);
    sum = actsum;
}


/** Função para adicionar itens a playlist */
void PlayList::addItems(const QStringList &parms) {
    bool select = false;
    QString isplay;
    QStringList files;

    /** Hack para o mouse não ocultar no diálogo para abrir arquivos */
    for (int i = 0; i < 1000; i++) Utils::arrowMouse();

    if (parms.isEmpty()) files = \
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
    else
        files = parms;

    int a = 0;
    int t = model->rowCount(QModelIndex());
    int total = files.size();

    if (files.isEmpty()) return;
    for (int i = 0; i < files.size(); ++i) {

        const QString &file = files.at(i);
        QString suffix = QFileInfo(file).suffix().toLower();

        if (!QFileInfo(file).isFile()) continue;
        else if (suffix == "m3u8" || suffix == "m3u") {
            load_m3u(file, DetectFormat);
            return;
        }
        else {
            if (hashlist.filter(QRegExp("^(" + Utils::stringHash(file) + ")$")).isEmpty()) {
                hashlist.append(Utils::stringHash(file));
                insert(file, a + t);
                a++;
            } else {
                total--;
                continue;
            }

            if (!select) {
                isplay = file;
                select = true;
            }
        }
    }

    save();
    if (total == 0) return;
    emit firstPlay(isplay, t);
    emit emitItems();
}


/** Função para carregar uma playlist no formato m3u/m3u8 */
void PlayList::load_m3u(const QString& file, M3UFormat format) {
    int i = 0;
    bool utf8;
    double duration;
    QString line, name, filename;

    /** Verificando codificação */
    if (format == DetectFormat) utf8 = (QFileInfo(file).suffix().toLower() == "m3u8");
    else utf8 = (format == M3U8);

    QRegExp m3u_id("^#EXTM3U|^#M3U");
    QRegExp rx_info("^#EXTINF:([.\\d]+).*tvg-logo=\"(.*)\",(.*)");

    QFile f(file);
    if (f.open(QIODevice::ReadOnly)) {
        clearItems();
        QString playlist_path = QFileInfo(file).path();

        QTextStream stream( &f );
        if (utf8) stream.setCodec("UTF-8");
        else stream.setCodec(QTextCodec::codecForLocale());

        while ( !stream.atEnd() ) {
            line = stream.readLine().trimmed();
            if (line.isEmpty()) continue;

            if (m3u_id.indexIn(line) != -1 || line.startsWith("#EXTVLCOPT:") || line.startsWith("#")) continue;
            else if (rx_info.indexIn(line) != -1) {
                duration = rx_info.cap(1).toDouble();
                name = rx_info.cap(3);
            }
            else if (line.startsWith("#EXTINF:")) {
                QStringList fields = line.mid(8).split(",");
                if (fields.count() >= 1) duration = fields[0].toDouble();
                if (fields.count() >= 2) name = fields[1];
            } else {
                filename = line;
                QFileInfo fi(filename);

                if (fi.exists()) {
                    filename = fi.absoluteFilePath();
                } else if (QFileInfo::exists(playlist_path + "/" + filename)) {
                    filename = playlist_path + "/" + filename;
                }

                name.replace("&#44;", ",");

                if (QFileInfo::exists(filename)) {
                    insert(filename, i, qint64(duration));
                    i++;
                }
            }
        }
        f.close();
        save();
        emit firstPlay(getItems(0), 0);
        emit emitItems();
    }
}


/** Adiciona os itens para salvar na playlist */
void PlayList::insert(const QString &url, int row, qint64 duration, const QString &format) {
    if (duration == 0)
        qDebug("%s(%sDEBUG%s):%s Adicionando %s ...\033[0m", GRE, RED, GRE, RDL, qUtf8Printable(url));
    else
        qDebug("%s(%sDEBUG%s):%s Atualizando %s ...\033[0m", GRE, RED, GRE, UPD, qUtf8Printable(url));
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
    if (duration != 0) listView->setCurrentIndex(model->index(row));
}


/** Adiciona os itens para serem visualizados na playlist */
void PlayList::insertItemAt(const PlayListItem &item, int row) {
    if (maxRows > 0 && model->rowCount(QModelIndex()) >= maxRows)
        model->removeRows(maxRows, model->rowCount(QModelIndex()) - maxRows + 1, QModelIndex());

    int i = model->items().indexOf(item, row + 1);
    if (i > 0) model->removeRow(i);
    if (!model->insertRow(row)) return;

    if (row > 0) {
        i = model->items().lastIndexOf(item, row - 1);
        if (i >= 0) model->removeRow(i);
    }
    setItemAt(item, row);
}


/** Localiza um item para exibir na playlist */
void PlayList::setItemAt(const PlayListItem &item, int row) {
    model->setData(model->index(row), QVariant::fromValue(item), Qt::DisplayRole);
}


/** Retorna o número do item que está atualmente em execução */
int PlayList::selectItems() {
    if (listView->currentIndex().row() == -1) return 0;
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
    auto pli = qvariant_cast<PlayListItem>(index.data(Qt::DisplayRole));
    return pli.url();
}


/** Função para retornar o número total de itens da playlist */
int PlayList::setListSize() {
    return model->rowCount(QModelIndex());
}


/** Função para remover apenas os itens selecionados */
void PlayList::removeSelectedItems(bool update) {
    QItemSelectionModel *selection = listView->selectionModel();
    if (!selection->hasSelection()) return;

    QModelIndexList s = selection->selectedIndexes();
    for (int i = s.size() - 1; i >= 0; --i) {
        if (!update) emit emitremove(s.at(i).row());
        hashlist.removeOne(Utils::stringHash(getItems(s.at(i).row())));
        model->removeRow(s.at(i).row());
    }
    delegate->resetWith();
    save();
}


/** Limpando os itens da playlist */
void PlayList::clearItems() {
    model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());
    delegate->resetWith();
    hashlist.clear();
    emit emitstop();
    emit emitItems();
    save();
}


/** Setando a duração do item atual */
qint64 PlayList::setDuration() {
    QModelIndex index = listView->currentIndex();
    auto pli = qvariant_cast<PlayListItem>(index.data(Qt::DisplayRole));
    return pli.duration();
}


/** Função para emitir o intem selecionado na playlist para execução com um duplo clique */
void PlayList::onAboutToPlay(const QModelIndex &index) {
    emit aboutToPlay(index.data(Qt::DisplayRole).value<PlayListItem>().url());
}


/** Função para emitir o intem selecionado na playlist */
void PlayList::onSelect(const QModelIndex &index) {
    emit selected(int(index.row()));
}


/** Emissão de sinal ao posicionar o mouse nos itens da playlist */
void PlayList::enterList() {
    emit enterListView();
}


/** Emissão de sinal ao posicionar o mouse fora dos itens da playlist */
void PlayList::leaveList() {
    emit leaveListView();
}


/** Função para recarregar os ícones da playlist */
void PlayList::changeIcons() {
    Utils::changeIcon(addBtn, "add");
    Utils::changeIcon(removeBtn, "remove");
    Utils::changeIcon(clearBtn, "clean");
}


/**********************************************************************************************************************/


/** Mapeador de eventos que está servindo para ocultar e desocultar a playlist */
void PlayList::mouseMoveEvent(QMouseEvent *event) {
    if (event->x() > (this->width() - 300) && event->y() < this->height() - 8 && !isshow) {
        qDebug("%s(%sDEBUG%s):%s Mouse posicionado na playlist ...\033[0m", GRE, RED, GRE, VIO);
        emit emitnohide();
        wpls->setVisible(true);
        isshow = true;
    } else if ((event->x() < (this->width() - 300) || event->y() > this->height() - 8) && isshow) {
        emit emithide();
        wpls->setVisible(false);
        isshow = false;
    }
    QWidget::mouseMoveEvent(event);
}


/** Emissão que serve como assistência para ocultar os controles */
bool PlayList::event(QEvent *event) {
    if (int(event->type()) == 110 && !isshow) emit emithiden();
    return QWidget::event(event);
}
