#include <QMouseEvent>
#include <Utils>

#include "ListView.h"


/**********************************************************************************************************************/


/** Construtor */
ListView::ListView(QStyledItemDelegate *delegate, QWidget *parent) : QListView(parent) {
    setSelectionMode(QAbstractItemView::ExtendedSelection); /** Uso com CTRL/SHIF */
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setStyleSheet(Utils::setStyle("playlist"));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setItemDelegate(delegate);
}


/** Destrutor */
ListView::~ListView() = default;


/**********************************************************************************************************************/


/** Gambiarra pra emitir o item selecionado ao pressionar o botão direito do mause */
void ListView::contextMenuEvent(QContextMenuEvent *event) {
    Q_EMIT clickedRight(currentIndex());
    QListView::contextMenuEvent(event);
}
