#include <QMouseEvent>
#include <Utils>

#include "CMenu.hpp"


/**********************************************************************************************************************/


/** Menu custamizado com direito a efeito de esmaecimento */
CMenu::CMenu(const QString &title, QWidget *parent) : QMenu(title, parent) {

    /** Efeito de transparência funcional. O setWindowOpacity() não rola. */
    effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(opacy);
    setGraphicsEffect(effect);

    setStyleSheet(Utils::setStyle("contextmenu"));
}


/** Destrutor */
CMenu::~CMenu() = default;


/**********************************************************************************************************************/


/** Interceptação ao abrir o menu de contexto */
void CMenu::showEvent(QShowEvent *event) {
    delete animation;
    animation = new QPropertyAnimation(effect, "opacity");
    Utils::fadeDiag(animation, 0, opacy);
    QWidget::showEvent(event);
}
