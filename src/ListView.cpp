#include <QDebug>

#include "ListView.h"
#include "PlayListDelegate.h"


/**********************************************************************************************************************/


/** Construtor para a listview */
ListView::ListView() {
    setItemDelegate(new PlayListDelegate(this));
    setSelectionMode(QAbstractItemView::ExtendedSelection); /** Uso com CTRL/SHIF */
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setFixedWidth(280);
}


/** Destrutor */
ListView::~ListView() = default;


/**********************************************************************************************************************/


/** Ação ao posicionar o mouse sobre a listview */
void ListView::enterEvent(QEvent *event) {
    qDebug() << "\033[32m(\033[31mDEBUG\033[32m):\033[35m Mouse posicionado na playlist ...\033[0m";
    emitEnter();
}


/** Ação ao desposicionar o mouse sobre a listview */
void ListView::leaveEvent(QEvent *event) {
    emitLeave();
}
