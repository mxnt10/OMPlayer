#include <QApplication>
#include <QDebug>
#include <QDomDocument>
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

#include "EventFilter.hpp"
#include "Extensions.hpp"
#include "PlayList.h"


/**********************************************************************************************************************/


/** Construtor do painel da playlist */
PlayList::PlayList(QWidget *parent) : QWidget(parent) {
    this->setMouseTracking(true);
    model = new PlayListModel(this);
    delegate = new PlayListDelegate(this);
    load(PlayList::First);


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
    auto *filter = new EventFilter(this, EventFilter::ControlEvent);
    listView = new ListView(delegate);
    listView->setModel(model);
    listView->installEventFilter(filter);
    connect(listView, &ListView::doubleClicked, this, &PlayList::onAboutToPlay);
    connect(listView, &ListView::clicked, this, &PlayList::onSelect);
    connect(listView, &ListView::clickedRight, this, &PlayList::onSelect);
    connect(filter, &EventFilter::emitEnter, [this](){ Q_EMIT enterListView(); });
    connect(filter, &EventFilter::emitLeave, [this](){ Q_EMIT leaveListView(); });


    /** Botões para o painel da playlist */
    addBtn = new Button(Button::NormalBtn, 32, "add");
    removeBtn = new Button(Button::NormalBtn, 32, "remove");
    clearBtn = new Button(Button::NormalBtn, 32, "clean");
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
    auto *filter2 = new EventFilter(this, EventFilter::ControlEvent);
    auto size = new QFrame();
    size->setFixedWidth(10);
    size->installEventFilter(filter2);
    connect(filter2, &EventFilter::emitEnter, [](){ resizeMouse(); });
    connect(filter2, &EventFilter::emitLeave, [](){ arrowMouse(); });


    /** Layout da playlist */
    wpls = new TrackWidget();
    cleanlist = new EmptyList(tr("Empty List"), wpls);
    auto line = new Line::Frame(Line::Frame::Horizontal);
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

    for (const auto &file : files) {
        QString suffix = QFileInfo(file).suffix().toLower();

        if (!QFileInfo(file).isFile()) continue;
        else if (QString::compare(suffix, "m3u8", Qt::CaseInsensitive) == 0 ||
                 QString::compare(suffix, "m3u", Qt::CaseInsensitive) == 0) {
            load_m3u(file, PlayList::DetectFormat);
            return;
        } else if (QString::compare(suffix, "xspf", Qt::CaseInsensitive) == 0) {
            loadXSPF(file);
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


/** Função para abrir playlist no formato xspf */
void PlayList::loadXSPF(const QString &filename) {
    int i = 0;
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly)) return;

    QDomDocument dom_document;
    if (!dom_document.setContent(f.readAll())) return;

    QDomNode root = dom_document.documentElement();
    QDomNode child = root.firstChildElement("trackList");

    if (!child.isNull()) {
        clearItems();

        QDomNode track = child.firstChildElement("track");
        while (!track.isNull()) {
            QString url = QUrl::fromPercentEncoding(
                    track.firstChildElement("location").text().toLatin1()).remove("file://");
            int duration = track.firstChildElement("duration").text().toInt();

            if (QFileInfo::exists(url)) {
                insert(url, i, qint64(duration));
                i++;
            }

            track = track.nextSiblingElement("track");
        }

        /** Verificando visibilidade de lista vazia */
        if (setListSize() == 0) cleanlist->setVisible(true);
        else cleanlist->setVisible(false);

        save();
        Q_EMIT firstPlay(getItems(0), 0);
    }
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

//bool Playlist::save_m3u(QString file) {
//    qDebug() << "Playlist::save_m3u:" << file;
//
//    QString dir_path = QFileInfo(file).path();
//    if (!dir_path.endsWith("/")) dir_path += "/";
//
//#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
//        dir_path = Helper::changeSlashes(dir_path);
//#endif
//
//    qDebug() << "Playlist::save_m3u: dir_path:" << dir_path;
//
//    bool utf8 = (QFileInfo(file).suffix().toLower() == "m3u8");
//
//    QFile f( file );
//    if ( f.open( QIODevice::WriteOnly ) ) {
//        QTextStream stream( &f );
//
//        if (utf8)
//            stream.setCodec("UTF-8");
//        else
//            stream.setCodec(QTextCodec::codecForLocale());
//
//        QString filename;
//        QString name;
//
//        stream << "#EXTM3U" << "\n";
//        stream << "# Playlist created by SMPlayer " << Version::printable() << " \n";
//
//        for (int n = 0; n < count(); n++) {
//            PLItem * i = itemData(n);
//            filename = i->filename();
//#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
//            filename = Helper::changeSlashes(filename);
//#endif
//            name = i->name();
//            name.replace(",", "&#44;");
//            QString icon_url = i->iconURL();
//            stream << "#EXTINF:";
//            stream << i->duration();
//            if (!icon_url.isEmpty()) stream << " tvg-logo=\"" + icon_url + "\"";
//            stream << ",";
//            stream << name << "\n";
//
//            // Save extra params
//            QStringList params = i->extraParams();
//                    foreach(QString par, params) {
//                    stream << "#EXTVLCOPT:" << par << "\n";
//                }
//
//            // Try to save the filename as relative instead of absolute
//            if (filename.startsWith( dir_path )) {
//                filename = filename.mid( dir_path.length() );
//            }
//            stream << filename << "\n";
//        }
//        f.close();
//
//        setPlaylistFilename(file);
//        setModified( false );
//        return true;
//    } else {
//        return false;
//    }
//}
//
//
//bool Playlist::save_pls(QString file) {
//    qDebug() << "Playlist::save_pls:" << file;
//
//    QString dir_path = QFileInfo(file).path();
//    if (!dir_path.endsWith("/")) dir_path += "/";
//
//#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
//        dir_path = Helper::changeSlashes(dir_path);
//#endif
//
//    qDebug() << "Playlist::save_pls: dir_path:" << dir_path;
//
//    QSettings set(file, QSettings::IniFormat);
//    set.beginGroup( "playlist");
//
//    QString filename;
//
//    for (int n = 0; n < count(); n++) {
//        PLItem * i = itemData(n);
//        filename = i->filename();
//#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
//        filename = Helper::changeSlashes(filename);
//#endif
//
//        // Try to save the filename as relative instead of absolute
//        if (filename.startsWith( dir_path )) {
//            filename = filename.mid( dir_path.length() );
//        }
//
//        set.setValue("File"+QString::number(n+1), filename);
//        set.setValue("Title"+QString::number(n+1), i->name());
//        set.setValue("Length"+QString::number(n+1), (int) i->duration());
//    }
//
//    set.setValue("NumberOfEntries", count());
//    set.setValue("Version", 2);
//
//    set.endGroup();
//
//    set.sync();
//
//    bool ok = (set.status() == QSettings::NoError);
//    if (ok) {
//        setPlaylistFilename(file);
//        setModified( false );
//    }
//
//    return ok;
//}
//
//bool Playlist::saveXSPF(const QString & filename) {
//    qDebug() << "Playlist::saveXSPF:" << filename;
//
//    QFile f(filename);
//    if (f.open( QIODevice::WriteOnly)) {
//        QTextStream stream(&f);
//        stream.setCodec("UTF-8");
//
//        stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
//        stream << "<playlist version=\"1\" xmlns=\"http://xspf.org/ns/0/\">\n";
//        stream << "\t<trackList>\n";
//
//        for (int n = 0; n < count(); n++) {
//            PLItem * i = itemData(n);
//            QString location = i->filename();
//            qDebug() << "Playlist::saveXSPF:" << location;
//
//            bool is_local = QFile::exists(location);
//
//#ifdef Q_OS_WIN
//            if (is_local) {
//				location.replace("\\", "/");
//			}
//#endif
//            //qDebug() << "Playlist::saveXSPF:" << location;
//
//            QUrl url(location);
//            location = url.toEncoded();
//            //qDebug() << "Playlist::saveXSPF:" << location;
//
//            if (!location.startsWith("file:") && is_local) {
//#ifdef Q_OS_WIN
//                location = "file:///" + location;
//#else
//                location = "file://" + location;
//#endif
//            }
//
//            QString title = i->name();
//            int duration = i->duration() * 1000;
//
//#if QT_VERSION >= 0x050000
//            location = location.toHtmlEscaped();
//            title = title.toHtmlEscaped();
//#else
//            location = Qt::escape(location);
//			title = Qt::escape(title);
//#endif
//
//            stream << "\t\t<track>\n";
//            stream << "\t\t\t<location>" << location << "</location>\n";
//            stream << "\t\t\t<title>" << title << "</title>\n";
//            stream << "\t\t\t<duration>" << duration << "</duration>\n";
//            stream << "\t\t</track>\n";
//        }
//
//        stream << "\t</trackList>\n";
//        stream << "</playlist>\n";
//
//        setPlaylistFilename(filename);
//        setModified(false);
//        return true;
//    } else {
//        return false;
//    }
//}

//bool Playlist::save(const QString & filename) {
//    qDebug() << "Playlist::save:" << filename;
//
//    QString s = filename;
//
//    if (s.isEmpty()) {
//        Extensions e;
//        s = MyFileDialog::getSaveFileName(
//                this, tr("Choose a filename"),
//                lastDir(),
//                tr("Playlists") + e.playlist().forFilter() + ";;" + tr("All files") +" (*)");
//    }
//
//    if (!s.isEmpty()) {
//        // If filename has no extension, add it
//        if (QFileInfo(s).suffix().isEmpty()) {
//            s = s + ".m3u";
//        }
//        if (QFileInfo(s).exists()) {
//            int res = QMessageBox::question( this,
//                                             tr("Confirm overwrite?"),
//                                             tr("The file %1 already exists.\n"
//                                                "Do you want to overwrite?").arg(s),
//                                             QMessageBox::Yes,
//                                             QMessageBox::No,
//                                             QMessageBox::NoButton);
//            if (res == QMessageBox::No ) {
//                return false;
//            }
//        }
//        latest_dir = QFileInfo(s).absolutePath();
//
//        QString suffix = QFileInfo(s).suffix().toLower();
//        if (suffix  == "pls") {
//            return save_pls(s);
//        }
//        else
//        if (suffix  == "xspf") {
//            return saveXSPF(s);
//        }
//        else {
//            return save_m3u(s);
//        }
//
//    } else {
//        return false;
//    }
//}

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
    if (resize) {
        if (*QApplication::overrideCursor() != QCursor(Qt::SizeHorCursor)) resizeMouse();
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
