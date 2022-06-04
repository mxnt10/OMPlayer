#include "Defines.h"
#include "Label.h"


/**********************************************************************************************************************/


/** Contrutor para o Label */
Label::Label(const QFlag& align, int w, const QString& tooltip, const QString& text) {
    setAlignment(align);
    txt = tooltip;

    if (w > 0)
        setFixedWidth(w);

    if (!tooltip.isEmpty())
        setToolTip(tooltip);

    if (!text.isEmpty())
        setText(text);
}


/** Destrutor */
Label::~Label() = default;


/**********************************************************************************************************************/


/** Ação ao posicionar o mouse sobre o label */
void Label::enterEvent(QEvent *event) {
    qDebug("%s(%sDEBUG%s):%s Mouse posicionado no label %s ...\033[0m", GRE, RED, GRE, VIO, qUtf8Printable(txt));
    emit emitEnter();
    QWidget::enterEvent(event);
}


/** Ação ao desposicionar o mouse sobre o label */
void Label::leaveEvent(QEvent *event) {
    emit emitLeave();
    QWidget::leaveEvent(event);
}
