#include <QMouseEvent>

#include "ClickableMenu.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Construtor que possibilita colocar o título do menu */
ClickableMenu::ClickableMenu(const QString &title, QWidget *parent) : QMenu(title, parent) {
    setWindowOpacity(0.8);
    setStyleSheet(Utils::setStyle("action"));
}


/** Destrutor */
ClickableMenu::~ClickableMenu() = default;


/**********************************************************************************************************************/


/** Para o funcionamento do menu clicavel. Evita o pressionamento completo, evitando o clique e por consequência,
 * o fechamento do menu de contexto. */
void ClickableMenu::mouseReleaseEvent(QMouseEvent *event) {
	QAction *action = actionAt(event->pos());
	if (action) {
		action->activate(QAction::Trigger);
		return;
	}
	QMenu::mouseReleaseEvent(event);
}


/**********************************************************************************************************************/


/** Construtor que possibilita colocar o título do menu */
CMenu::CMenu(const QString &title, QWidget *parent) : QMenu(title, parent) {
    setWindowOpacity(0.8);
    setStyleSheet(Utils::setStyle("contextmenu"));
}


/** Destrutor */
CMenu::~CMenu() = default;
