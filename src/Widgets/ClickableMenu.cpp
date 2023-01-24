#include <QMouseEvent>
#include <Utils>

#include "ClickableMenu.hpp"


/**********************************************************************************************************************/


/** Construtor que possibilita colocar o título do menu */
ClickableMenu::ClickableMenu(const QString &title, QWidget *parent) : QMenu(title, parent) {

    /** Efeito de transparência funcional. O setWindowOpacity() não rola. */
    effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(0.8);
    setGraphicsEffect(effect);

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


/** Interceptação ao fechar o menu de contexto */
void ClickableMenu::closeEvent(QCloseEvent *event) {
    if (onclose) {
        event->accept();
        return;
    }
    Utils::fadeDiag(animation, 0.8, 0);
    connect(animation, &QPropertyAnimation::finished, [this](){
        onclose = true;
        this->close();
    });
    event->ignore();
}


/** Interceptação ao abrir o menu de contexto */
void ClickableMenu::showEvent(QShowEvent *event) {
    delete animation;
    animation = new QPropertyAnimation(effect, "opacity");
    onclose = false;
    Utils::fadeDiag(animation, 0, 0.8);
    QWidget::showEvent(event);
}
