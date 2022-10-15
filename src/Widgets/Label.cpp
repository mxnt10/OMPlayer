#include <QDebug>
#include <Utils>

#include "Label.h"


/**********************************************************************************************************************/


/** Contrutor para o Label */
Label::Label(const QFlag& align, int w, const QString& text) {
    setAlignment(align);
    if (w > 0) setFixedWidth(w);
    if (!text.isEmpty()) setText(text);
}


/** Contrutor com parent */
Label::Label(const QFlag &align, int w, int h, QWidget *parent) : QLabel(parent) {
    setAlignment(align);
    if (w > 0) setFixedWidth(w);
    if (h > 0) setFixedHeight(h);
}


/** Construtor para a imagem */
Label::Label(const QFlag& align, const QString &local) {
    setAlignment(align);
    setPixmap(QPixmap(local));
}


/** Destrutor */
Label::~Label() = default;


/**********************************************************************************************************************/


/** Ação ao posicionar o mouse sobre o label */
void Label::enterEvent(QEvent *event) {
    qDebug("%s(%sLabel%s)%s::%sMouse posicionado em um label ...\033[0m", GRE, RED, GRE, RED, VIO);
    QLabel::enterEvent(event);
}
