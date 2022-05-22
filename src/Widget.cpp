#include "Widget.h"


/**********************************************************************************************************************/


/** Construtor */
Widget::Widget(QWidget *parent) : QWidget(parent) {}


/** Destrutor */
Widget::~Widget() = default;


/**********************************************************************************************************************/


/** Ação ao posicionar o mouse sobre o widget */
void Widget::enterEvent(QEvent *event) {
    emit emitEnter();
    QWidget::enterEvent(event);
}


/** Ação ao desposicionar o mouse sobre o widget */
void Widget::leaveEvent(QEvent *event) {
    emit emitLeave();
    QWidget::leaveEvent(event);
}
