#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QLayout>
#include <QMoveEvent>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QStandardPaths>
#include <QTextCodec>
#include <QTimer>
#include <Frame>
#include <Utils>

#include "EventFilter.h"
#include "Extensions.h"
#include "PlayList.h"


/**********************************************************************************************************************/


/** Construtor do painel da playlist */
PlayList::PlayList(QWidget *parent) : QWidget(parent) {
    this->setMouseTracking(true);
    model = new PlayListModel(this);
    delegate = new PlayListDelegate(this);
    load(First);


    /** Criação do diálogo para abrir arquivos de mídia */
    Extensions e;
    diag = new QFileDialog(this);
    diag->setFileMode(QFileDialog::ExistingFiles);
    diag->setNameFilter(e.filters());
    diag->setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()));


    /** Efeito de transparência funcional para a playlist. O setWindowOpacity() não rolou. */
    effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(1.0);
    setGraphicsEffect(effect);


    /** Lista para visualização da playlist */
    auto *filter = new EventFilter(this, EventFilter::Control);
    listView = new ListView();
    listView->setModel(model);
    listView->setItemDelegate(delegate);
    listView->installEventFilter(filter);
    connect(listView, &ListView::doubleClicked, this, &PlayList::onAboutToPlay);
    connect(listView, &ListView::clicked, this, &PlayList::onSelect);
    connect(listView, &ListView::clickedRight, this, &PlayList::onSelect);
    connect(filter, &EventFilter::emitEnter, [this](){ Q_EMIT enterListView(); });
    connect(filter, &EventFilter::emitLeave, [this](){ Q_EMIT leaveListView(); });


    /** Botões para o painel da playlist */
    addBtn = new Button(Button::Default, 32, "add");
    removeBtn = new Button(Button::Default, 32, "remove");
    clearBtn = new Button(Button::Default, 32, "clean");
    connect(addBtn, &Button::clicked, this, &PlayList::getFiles);
    connect(removeBtn, &Button::clicked, this, &PlayList::removeSelectedItems);
    connect(clearBtn, &Button::clicked, this, &PlayList::clearItems);


    /** Plano de fundo da playlist */
    auto *bgpls = new QWidget();
    bgpls->setStyleSheet(Utils::setStyle("widget"));


    /** Organização dos botões */
    auto *hbtn = new QHBoxLayout();
    hbtn->addWidget(addBtn);
    hbtn->addSpacing(2);
    hbtn->addWidget(removeBtn);
    hbtn->addStretch(1);
    hbtn->addWidget(clearBtn);


    /** Gerando uma linha vertical transparente */
    auto *filter2 = new EventFilter(this, EventFilter::Control);
    auto size = new QFrame();
    size->setFixedWidth(10);
    size->installEventFilter(filter2);
    connect(filter2, &EventFilter::emitEnter, [](){ resizeMouse(); });
    connect(filter2, &EventFilter::emitLeave, [](){ arrowMouse(); });


    /** Mensagem para a playlist vazia e uma linha horizontal */
    cleanlist = new EmptyList(tr("Empty List"), wpls);
    auto line = new Line::Frame(Line::Frame::Horizontal);


    /** Layout da playlist */
    wpls = new QWidget();
    wpls->setMouseTracking(true);
    auto *vbl = new QVBoxLayout();
    vbl->addLayout(hbtn);
    vbl->addSpacing(5);
    vbl->addWidget(line);
    vbl->addSpacing(5);
    vbl->addWidget(listView);


    /** Layout com a barra de redirecionamento */
    auto *hbl = new QHBoxLayout(wpls);
    hbl->setContentsMargins(0, 10, 10, 10);
    hbl->setSpacing(0);
    hbl->addWidget(size);
    hbl->addLayout(vbl);


    /** Layout do painel da playlist */
    auto *gbl = new QGridLayout();
    gbl->setMargin(10);
    gbl->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding), 0, 0, 1, 2);
    gbl->addWidget(bgpls, 0, 1, 1, 3);
    gbl->addWidget(wpls, 0, 1, 1, 3);
    gbl->setAlignment(RIGHT);
    wpls->setVisible(false);
    setLayout(gbl);


    /** Definindo o tamanho da playlist */
    if (JsonTools::intJson("pls_size") > 0) {
        wpls->setFixedWidth(JsonTools::intJson("pls_size"));
        listView->setFixedWidth(JsonTools::intJson("pls_listsize"));
        delegate->setWith(JsonTools::intJson("pls_size"));
    } else listView->setFixedWidth(280);


    /** Desativando menu de contexto dos scroolbar */
    foreach(QObject *widget, qApp->allWidgets()) {
        auto *scrollBar = dynamic_cast<QScrollBar*>(widget);
        if(scrollBar) scrollBar->setContextMenuPolicy(Qt::NoContextMenu);
    }
}


/** Destrutor necessário */
PlayList::~PlayList() = default;


/**********************************************************************************************************************/


/** Função para carregar a playlist ao abrir */
void PlayList::load(PlayList::STATUS load, const QString &url) {
    QFile f(url);
    if (!f.exists()) save();
    if (!f.open(QIODevice::ReadOnly)) return;

    qDebug("%s(%sPlaylist%s)%s::%sCarregando a playlist ...\033[0m", GRE, RED, GRE, RED, ORA);
    if (load == PlayList::Second) model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());
    QDataStream ds(&f);
    QList<PlayListItem> list;
    ds >> list;

    int add = 0;
    for (const auto &i: list) {
        if (QFileInfo::exists(i.url())) {
            insertItemAt(i, add);
            add++;
        }
    }

    f.close();
    if (load != First) save();
}


/** Função para salvar a playlist ao fechar */
void PlayList::save(const QString &url) {
    qDebug("%s(%sPlaylist%s)%s::%sSalvando a playlist ...\033[0m", GRE, RED, GRE, RED, ORA);
    QFile f(url);
    if (!f.open(QIODevice::WriteOnly)) return;
    QDataStream ds(&f);
    ds << model->items();
    f.close();
}


/** Usando uma função separada para buscar os arquivos */
void PlayList::getFiles() {
    Q_EMIT nomousehide();

    /** Hack para o mouse não ocultar no diálogo para abrir arquivos */
    for (int i = 0; i < 400; i++) arrowMouse();

    QStringList files;
    if (diag->exec()) files = diag->selectedFiles();
    if (!files.isEmpty()) addItems(files);
}


/** Função para adicionar itens a playlist */
void PlayList::addItems(const QStringList &files) {
    if (files.isEmpty()) {
        blankMouse();
        return;
    }

    int a = 0;
    int t = model->rowCount(QModelIndex());
    rmRows = 0;

    for (int i = 0; i < files.size(); ++i) {
        const QString &file = files.at(i);
        QString suffix = QFileInfo(file).suffix().toLower();

        if (!QFileInfo(file).isFile()) continue;
        else if (suffix == "m3u8" || suffix == "m3u") {
            load_m3u(file, DetectFormat);
            return;
        } else {
            insert(file, a + t - rmRows);
            a++;
        }
    }

    qDebug("%s(%sPlaylist%s)%s::%sItens inseridos na playlist:"
           "\n            - Existentes:  %s%4i%s"
           "\n            - Adicionados: %s%4i%s"
           "\n            - Repetidos:   %s%4i%s"
           "\n            - Total:       %s%4i"
           "\033[0m", GRE, RED, GRE, RED, BLU, YEL, t, BLU, GRE, a - rmRows,
           BLU, RDL, rmRows, BLU, VIO, a + t - rmRows);

    /** Verificando visibilidade de lista vazia */
    if (setListSize() == 0) cleanlist->setVisible(true);
    else cleanlist->setVisible(false);

    save();
    Q_EMIT firstPlay(getItems(t), t);
}


/** Função para carregar uma playlist no formato m3u/m3u8 */
void PlayList::load_m3u(const QString& file, M3UFormat format) {
    int i = 0;
    bool utf8;
    double duration;
    QString line, url;

    /** Verificando codificação */
    if (format == DetectFormat) utf8 = (QFileInfo(file).suffix().toLower() == "m3u8");
    else utf8 = (format == M3U8);

    QRegExp m3u_id("^#EXTM3U|^#M3U");
    QRegExp rx_info("^#EXTINF:([.\\d]+).*tvg-logo=\"(.*)\",(.*)");

    QFile f(file);
    if (f.open(QIODevice::ReadOnly)) {
        clearItems();
        QString playlist_path = QFileInfo(file).path();

        QTextStream stream(&f);
        if (utf8) stream.setCodec("UTF-8");
        else stream.setCodec(QTextCodec::codecForLocale());

        while ( !stream.atEnd() ) {
            line = stream.readLine().trimmed();
            if (line.isEmpty() || m3u_id.indexIn(line) != -1 || line.startsWith("#EXTVLCOPT:") || line.startsWith("#"))
                continue;
            else if (rx_info.indexIn(line) != -1) duration = rx_info.cap(1).toDouble();
            else if (line.startsWith("#EXTINF:")) {
                QStringList fields = line.mid(8).split(",");
                if (fields.count() >= 1) duration = fields[0].toDouble();
            } else {
                url = line;
                QFileInfo fi(url);

                if (fi.exists()) url = fi.absoluteFilePath();
                else if (QFileInfo::exists(playlist_path + "/" + url)) url = playlist_path + "/" + url;

                if (QFileInfo::exists(url)) {
                    insert(url, i, qint64(duration));
                    i++;
                }
            }
        }
        f.close();

        /** Verificando visibilidade de lista vazia */
        if (setListSize() == 0) cleanlist->setVisible(true);
        else cleanlist->setVisible(false);

        save();
        Q_EMIT firstPlay(getItems(0), 0);
    }
}


/** Adiciona os itens para salvar na playlist */
void PlayList::insert(const QString &url, int row, qint64 duration) {
    qDebug("%s(%sPlaylist%s)%s::%sAdicionando %s ...\033[0m", GRE, RED, GRE, RED, RDL, STR(url));
    PlayListItem item;
    item.setUrl(url);
    item.setDuration(duration);
    item.setFormat(nullptr);

    /** Definindo o título */
    if (!url.contains(QLatin1String("://")) || url.startsWith(QLatin1String("file://")))
        item.setTitle(QFileInfo(url).fileName());
    else item.setTitle(url);

    insertItemAt(item, row);
}


/** Adiciona os itens para serem visualizados na playlist */
void PlayList::insertItemAt(const PlayListItem &item, int row) {
    model->insertRow(row);

    /** Verificando itens repetidos */
    if (row > 0) {
        for (int j = 0; j < model->rowCount(QModelIndex()) + 1; j++) {
            if (model->items().lastIndexOf(item, j) >= 0) {
                model->removeRow(row);
                rmRows++;
                break;
            }
        }
    }

    model->setData(model->index(row), QVariant::fromValue(item), Qt::DisplayRole);
}


/** Atualizando itens da playlist */
void PlayList::updateItems(int row, qint64 duration, const QString &format) {
    QModelIndex index = model->index(row);
    auto pli = qvariant_cast<PlayListItem>(index.data(Qt::DisplayRole));
    pli.setDuration(duration);
    pli.setFormat(format);
    model->setData(model->index(row), QVariant::fromValue(pli), Qt::DisplayRole);
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


/** Setando a duração do item atual */
qint64 PlayList::setDuration() {
    QModelIndex index = listView->currentIndex();
    auto pli = qvariant_cast<PlayListItem>(index.data(Qt::DisplayRole));
    return pli.duration();
}


/** Função para retornar o número total de itens da playlist */
int PlayList::setListSize() { return model->rowCount(QModelIndex()); }


/** Função para remover apenas os itens selecionados */
void PlayList::removeSelectedItems() {
    QItemSelectionModel *selection = listView->selectionModel();
    if (!selection->hasSelection()) return;

    QModelIndexList s = selection->selectedIndexes();
    for (int i = s.size() - 1; i >= 0; --i) {
        model->removeRow(s.at(i).row());
        if (setListSize() == 0) cleanlist->setVisible(true);
        Q_EMIT emitremove(s.at(i).row());
    }
    save();
}


/** Limpando os itens da playlist */
void PlayList::clearItems() {
    qDebug("%s(%sPlaylist%s)%s::%sZerando a playlist ...\033[0m", GRE, RED, GRE, RED, RDL);
    model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());
    cleanlist->setVisible(true);
    Q_EMIT emitstop();
    save();
}


/** Função para emitir o item selecionado na playlist para execução com um duplo clique */
void PlayList::onAboutToPlay(const QModelIndex &index) {
    Q_EMIT aboutToPlay(index.data(Qt::DisplayRole).value<PlayListItem>().url());
}


/** Função para emitir o item selecionado na playlist */
void PlayList::onSelect(const QModelIndex &index) { Q_EMIT selected(int(index.row())); }


/** Função para recarregar os ícones da playlist */
void PlayList::changeIcons() {
    Utils::changeIcon(addBtn, "add");
    Utils::changeIcon(removeBtn, "remove");
    Utils::changeIcon(clearBtn, "clean");
}


/** Minimização antes dos diálogos */
void PlayList::hideFade() {
    Q_EMIT emithide();
    if (isshow) fadePls(Hide);
    QTimer::singleShot(130, [this](){ isshow = false; });
}


/** Efeito fade bacana para a playlist */
void PlayList::fadePls(PLS option) {
    auto *animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(FADE_VAL);

    if (option == Show) {
        animation->setStartValue(0);
        animation->setEndValue(1);
        wpls->setVisible(true);
    } else if (option == Hide) {
        animation->setStartValue(1);
        animation->setEndValue(0);

        /** A visibilidade deve mudar para evitar possíveis bugs */
        connect(animation, &QPropertyAnimation::finished, [this](){ wpls->setVisible(false); });
    }
    animation->start();
}


/**********************************************************************************************************************/


/** Mapeador de eventos que está servindo para ocultar e desocultar a playlist */
void PlayList::mouseMoveEvent(QMouseEvent *event) {
    if (*QApplication::overrideCursor() == QCursor(Qt::SizeHorCursor)) {
        int subsize = startpos - event->x();
        int size = startsize + subsize;
        if (size >= 300 && size < 876 && resize) {
            wpls->setFixedWidth(size);
            listView->setFixedWidth(startlistsize + subsize);
            delegate->setWith(size);
            cleanlist->move(size/2 - cleanlist->width()/2 + 10, 60);
        }
    } else {
        if (event->x() > (this->width() - wpls->width() - 20) && event->y() < this->height() - 8 && !isshow) {
            qDebug("%s(%sPlaylist%s)%s::%sMouse posicionado na playlist ...\033[0m", GRE, RED, GRE, RED, VIO);
            Q_EMIT emitnohide();
            fadePls(Show);
            isshow = true;

            /** Verificando visibilidade de lista vazia */
            cleanlist->move(wpls->width()/2 - cleanlist->width()/2 + 10, 60);
            if (setListSize() == 0) cleanlist->setVisible(true);
            else cleanlist->setVisible(false);

        } else if ((event->x() < (this->width() - wpls->width() - 20) || event->y() > this->height() - 8) && isshow) {
            arrowMouse();
            Q_EMIT emithide();
            fadePls(Hide);
            isshow = false;
        }
    }
    if (!resize && *QApplication::overrideCursor() == QCursor(Qt::SizeHorCursor)) arrowMouse();
    QWidget::mouseMoveEvent(event);
}


/** Emissão ao pressionar um botão do mouse */
void PlayList::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && *QApplication::overrideCursor() == QCursor(Qt::SizeHorCursor) &&
        event->x() < this->width() - wpls->width()) {
        resize = true;
        startpos = event->x();
        startsize = wpls->width();
        startlistsize = listView->width();
    }
    QWidget::mousePressEvent(event);
}


/** Emissão ao soltar um botão do mouse */
void PlayList::mouseReleaseEvent(QMouseEvent *event) {
    if (resize) {
        resize = false;
        JsonTools::intJson("pls_size", wpls->width());
        JsonTools::intJson("pls_listsize", listView->width());
    }
    QWidget::mouseReleaseEvent(event);
}
