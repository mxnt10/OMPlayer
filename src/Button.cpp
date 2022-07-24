#include <string>

#include "Button.h"
#include "JsonTools.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Construtor que define a classe dos botões do reprodutor */
Button::Button(const QString &icon, int size, const QString &tooltip, const QString &text, bool fixed) {
    num = size;
    fix = fixed;
    txt = text;
    setIconSize(QSize(num, num));
    setFocusPolicy(Qt::NoFocus);


    /** Para botões que agem feito radio buttons */
    if (fix) setFixedSize(num, num);
    if (!text.isEmpty()) setText(txt);


    /** Definindo ícone */
    QString theme = JsonTools::stringJson("theme");
    if (Utils::setIconTheme(theme, icon) == nullptr)
        setIcon(QIcon::fromTheme(Utils::defaultIcon(icon)));
    else setIcon(QIcon(Utils::setIconTheme(theme, icon)));


    /** Apenas para o Debug */
    std::string upper = icon.toStdString();
    std::transform(upper.begin(), upper.end(), upper.begin(), ::tolower);
    upper[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(upper[0]))); /** C++ 17 */
    ico = QString::fromStdString(upper);


    /** ToolTip */
    if (tooltip.isEmpty()) setToolTip(ico);
    else if (QString::compare(tooltip, NOTOOLTIP, Qt::CaseSensitive) != 0) setToolTip(tooltip);
}


/** Destrutor */
Button::~Button() = default;


/**********************************************************************************************************************/


/** Ação ao posicionar o mouse sobre o botão */
void Button::enterEvent(QEvent *event) {
    emit emitEnter();

    if (fix) {
        qDebug("%s(%sDEBUG%s):%s Mouse posicionado no botão %s ...\033[0m", GRE, RED, GRE, VIO, qUtf8Printable(ico));
        setIconSize(QSize(num + 2, num + 2));
    } else {
        qDebug("%s(%sDEBUG%s):%s Mouse posicionado na seleção %s ...\033[0m", GRE, RED, GRE, VIO, qUtf8Printable(txt));
        setText(" " + txt);
    }
    QPushButton::enterEvent(event);
}


/** Ação ao desposicionar o mouse sobre o botão */
void Button::leaveEvent(QEvent *event) {
    if (fix) setIconSize(QSize(num, num));
    else setText(txt);
    QPushButton::leaveEvent(event);
}


/** Iniciando o efeito do botão */
void Button::mousePressEvent(QMouseEvent *event) {
    if (fix) {
        qDebug("%s(%sDEBUG%s):%s Pressioando o botão %s ...\033[0m", GRE, RED, GRE, VIO, qUtf8Printable(ico));
        setIconSize(QSize(num - 2, num - 2));
    }
    QPushButton::mousePressEvent(event);
}


/** Finalizando o efeito do botão */
void Button::mouseReleaseEvent(QMouseEvent *event) {
    if (fix) setIconSize(QSize(num + 2, num + 2));
    QPushButton::mouseReleaseEvent(event);
}
