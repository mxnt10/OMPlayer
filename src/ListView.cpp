#include <QGraphicsOpacityEffect>

#include "Defines.h"
#include "ListView.h"
#include "PlayListDelegate.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Construtor para a listview */
ListView::ListView() {
    setItemDelegate(new PlayListDelegate(this));
    setSelectionMode(QAbstractItemView::ExtendedSelection); /** Uso com CTRL/SHIF */
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setStyleSheet(Utils::setStyle("playlist"));
    setFixedWidth(280);

    /** Efeito semitransparente para o fundo da playlist */
    auto *effect = new QGraphicsOpacityEffect();
    effect->setOpacity(OPACY);
    setGraphicsEffect(effect);
}


/** Destrutor */
ListView::~ListView() = default;


/**********************************************************************************************************************/


/** Ação ao posicionar o mouse sobre a listview */
void ListView::enterEvent(QEvent *event) {
    emit emitEnter();
    QListView::enterEvent(event);
}
