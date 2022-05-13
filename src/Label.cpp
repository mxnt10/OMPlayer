#include "Defines.h"
#include "Label.h"


/**********************************************************************************************************************/


Label::Label(int w, const QString& tooltip, const QString& text) {
    setAlignment(CENTER);
    txt = tooltip;

    if ( w > 0)
        setFixedWidth(w);

    if (!tooltip.isEmpty())
        setToolTip(tooltip);

    if (!text.isEmpty())
        setText(text);
}

Label::~Label() = default;


/**********************************************************************************************************************/


/** Ação ao posicionar o mouse sobre o label */
void Label::enterEvent(QEvent *event) {
    qDebug("%s(%sDEBUG%s):%s Mouse posicionado no label %s ...\033[0m", GRE, RED, GRE, VIO, txt.toStdString().c_str());
    emit emitEnter();
    QWidget::enterEvent(event);
}


/** Ação ao desposicionar o mouse sobre o label */
void Label::leaveEvent(QEvent *event) {
    emit emitLeave();
    QWidget::leaveEvent(event);
}
