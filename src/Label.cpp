#include <QDebug>

#include "Label.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Contrutor para o Label */
Label::Label(const QFlag& align, int w, const QString& text) {
    setMouseTracking(true);
    setAlignment(align);

    if (w > 0) setFixedWidth(w);
    if (!text.isEmpty()) setText(text);
}


/** Contrutor para o Label */
Label::Label(const QFlag &align, int w, int h, QWidget *parent) : QLabel(parent) {
    setAlignment(align);

    if (w > 0) setFixedWidth(w);
    if (h > 0) setFixedHeight(h);
}


/** Destrutor */
Label::~Label() = default;


/**********************************************************************************************************************/


/** Ação ao posicionar o mouse sobre o label */
void Label::enterEvent(QEvent *event) {
    qDebug("%s(%sDEBUG%s):%s Mouse posicionado em um label ...\033[0m", GRE, RED, GRE, VIO);
    QLabel::enterEvent(event);
}
