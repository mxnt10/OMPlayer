#include <QDebug>
#include "Widget.h"


/**********************************************************************************************************************/


/** Ação ao posicionar o mouse sobre o widget */
void Widget::enterEvent(QEvent *event) {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[35m Mouse posicionado nos controles de reprodução ...\033[0m");
    emit emitEnter();
    QWidget::enterEvent(event);
}


/** Ação ao desposicionar o mouse sobre o widget */
void Widget::leaveEvent(QEvent *event) {
    emit emitLeave();
    QWidget::leaveEvent(event);
}
