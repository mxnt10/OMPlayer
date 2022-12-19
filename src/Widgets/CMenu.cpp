#include <QMouseEvent>
#include <Utils>

#include "CMenu.h"


/**********************************************************************************************************************/


/** Menu custamizado com direito a efeito de esmaecimento */
CMenu::CMenu(const QString &title, QWidget *parent) : QMenu(title, parent) {

    /** Efeito de transparência funcional. O setWindowOpacity() não rola. */
    effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(0.8);
    setGraphicsEffect(effect);

    setStyleSheet(Utils::setStyle("contextmenu"));
}


/** Destrutor */
CMenu::~CMenu() = default;


/**********************************************************************************************************************/


/** Interceptação ao fechar o menu de contexto */
void CMenu::closeEvent(QCloseEvent *event) {
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
void CMenu::showEvent(QShowEvent *event) {
    animation = nullptr;
    delete animation;
    animation = new QPropertyAnimation(effect, "opacity");
    onclose = false;
    Utils::fadeDiag(animation, 0, 0.8);
    QWidget::showEvent(event);
}
