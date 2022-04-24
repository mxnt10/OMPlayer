#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsOpacityEffect>
#include <QLayout>
#include <QFrame>
#include <QMoveEvent>
#include <QSpacerItem>

#include "Button.h"
#include "Defines.h"
#include "ListView.h"
#include "PlayList.h"
#include "PlayListModel.h"
#include "Utils.h"

using QSizePolicy::Expanding;


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


    /** Botões para o painel da playlist */
    addBtn = new Button("add", 32);
    addBtn->setToolTip(tr("Add items"));
    removeBtn = new Button("remove", 32);
    removeBtn->setToolTip(tr("Remove items"));
    clearBtn = new Button("clean", 32);
    clearBtn->setToolTip(tr("Clear playlist"));


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
    gbl->setAlignment(Qt::AlignRight);
    wpls->setVisible(false);
    setLayout(gbl);


    /** Eventos da playlist */
    connect(addBtn, SIGNAL(clicked()), SLOT(addItems()));
    connect(removeBtn, SIGNAL(clicked()), SLOT(removeSelectedItems()));
    connect(clearBtn, SIGNAL(clicked()), SLOT(clearItems()));
    connect(listView, SIGNAL(doubleClicked(QModelIndex)), SLOT(onAboutToPlay(QModelIndex)));
    connect(listView, SIGNAL(clicked(QModelIndex)), SLOT(onSelect(QModelIndex)));
    connect(listView, SIGNAL(emitEnter()), SLOT(noHide()));
}


/** Destrutor */
PlayList::~PlayList() {
    save();
}


/**********************************************************************************************************************/


/** Usado para setar a localização do arquivo da playlist para carregar e salvar a playlist */
void PlayList::setSaveFile(const QString &file) {
    mfile = file;
}


/** Função para carregar a playlist ao abrir */
void PlayList::load() {
    QFile f(mfile);
    if (!f.exists())
        return;
    if (!f.open(QIODevice::ReadOnly))
        return;
    QDataStream ds(&f);
    QList<PlayListItem> list;
    ds >> list;
    for (int i = 0; i < list.size(); ++i) {
        insertItemAt(list.at(i), i);
    }
}


/** Função para salvar a playlist ao fechar */
void PlayList::save() {
    QFile f(mfile);
    if (!f.open(QIODevice::WriteOnly))
        return;
    QDataStream ds(&f);
    ds << model->items();
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
        files = QFileDialog::getOpenFileNames(nullptr, tr("Select multimedia files"), QDir::homePath());
    else
        files = parms;

    if (files.isEmpty())
        return;
    for (int i = 0; i < files.size(); ++i) {
        const QString& file = files.at(i);
        if (!QFileInfo(file).isFile())
            continue;
        insert(file, i);
        if (!select) {
            isplay = file;
            select = true;
        }
    }
    emit firstPlay(isplay);
}


/** Adiciona os itens para salvar na playlist */
void PlayList::insert(const QString &url, int row, qint64 duration, const QString &format) {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[31m Adicionando %s...\033[0m", url.toStdString().c_str());
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
    model->setData(model->index(row), QVariant::fromValue(item), Qt::DisplayRole);
}


/** Retorna o número do item que está atualmente em execução */
int PlayList::selectItems() {
    if (listView->currentIndex().row() == -1)
        return 0;
    return listView->currentIndex().row();
}


/** Selecionando o primeiro item da lista */
void PlayList::selectPlay() {
    if (listView->currentIndex().row() == -1) {
        listView->setCurrentIndex(model->index(0));
        actualitem = listView->currentIndex();
    }
}


/** Selecionando o próximo item da lista */
void PlayList::selectNext() {
    if (actualitem.isValid())
        listView->setCurrentIndex(model->index(actualitem.row()));
    listView->setCurrentIndex(model->index(listView->currentIndex().row() + 1));
    if (listView->currentIndex().row() == -1)
        listView->setCurrentIndex(model->index(0));
    actualitem = listView->currentIndex();
}


/** Selecionando um item anterior da lista */
void PlayList::selectPrevious() {
    if (actualitem.isValid())
        listView->setCurrentIndex(model->index(actualitem.row()));
    listView->setCurrentIndex(model->index(listView->currentIndex().row() - 1));
    if (listView->currentIndex().row() == -1)
        listView->setCurrentIndex(model->index(setListSize() - 1));
    actualitem = listView->currentIndex();
}

void PlayList::selectCurrent(int indx) {
    listView->setCurrentIndex(model->index(indx));
    actualitem = listView->currentIndex();
}


/** Limpar item selecionado da playlist */
void PlayList::selectClean() {
    listView->setCurrentIndex(model->index(-1));
    actualitem = listView->currentIndex();
    listView->clearSelection();
}


/** Retorna uma url de um arquivo multimídia usando um valor inteiro especificado */
QString PlayList::getItems(int s) {
    save();
    QFile f(mfile);
    if (!f.open(QIODevice::ReadOnly))
        return{};
    QDataStream ds(&f);
    QList<PlayListItem> list;
    ds >> list;
    QString item;
    if ( s < list.size())
        item = list.at(s).url();
    else return{};
    return item;
}


/** Função para retornar o número total de itens da playlist */
int PlayList::setListSize() {
    return model->rowCount(QModelIndex());
}


/** Função para remover apenas os itens selecionados */
void PlayList::removeSelectedItems() {
    QItemSelectionModel *selection = listView->selectionModel();
    if (!selection->hasSelection())
        return;
    QModelIndexList s = selection->selectedIndexes();
    for (int i = s.size() - 1; i >= 0; --i) {
        model->removeRow(s.at(i).row());
    }
}


/** Limpando os itens da playlist */
void PlayList::clearItems() {
    model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());
}


/** Setando a duração do item atual */
qint64 PlayList::setDuration() {
    QModelIndex index = listView->currentIndex();
    auto pli = qvariant_cast<PlayListItem>(index.data(Qt::DisplayRole));
    return pli.duration();
}


/** Função para emitir o intem selecionado na playlist para execução com um duplo clique */
void PlayList::onAboutToPlay(const QModelIndex &index) {
    actualitem = listView->currentIndex();
    emit aboutToPlay(index.data(Qt::DisplayRole).value<PlayListItem>().url());
    save();
}


/** Função para emitir o intem selecionado na playlist */
void PlayList::onSelect(const QModelIndex &index) {
    emit selected(int(index.row()));
}


/** Assistente para setar o item atual ao selecionar um item da playlist apenas com a reprodução parada */
void PlayList::setIndex() {
    actualitem = listView->currentIndex();
}


/** Assistente para emissão de sinal */
void PlayList::noHide() {
    emit emitnohide();
}


/**********************************************************************************************************************/


/** Mapeador de eventos que está servindo para ocultar e desocultar a playlist */
void PlayList::mouseMoveEvent(QMouseEvent *event) {
    if (event->x() > (this->width() - 300) && !isshow) {
        emit emitnohide();
        wpls->setVisible(true);
        isshow = true;
    } else if (event->x() < (this->width() - 300) && isshow) {
        wpls->setVisible(false);
        isshow = false;
    }
}


/** Emissão que serve como assistência para ocultar os controles */
bool PlayList::event(QEvent *event) {
    if (int(event->type()) == 110 && !isshow)
        emit emithide();
    return QWidget::event(event);
}
