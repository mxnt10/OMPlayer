#include <QTimer>
#include <string>

#include "Button.h"
#include "JsonTools.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Construtor que define a classe dos botões do reprodutor */
Button::Button(const QString &icon, int size, const QString &tooltip, const QString &text, bool fixed) {
    ico = icon;
    num = size;
    fix = fixed;
    txt = text;
    setIconSize(QSize(num, num));
    setFocusPolicy(Qt::NoFocus);
    setStyleSheet("QPushButton { border: 0; }");

    if (fixed) setFixedSize(num, num);
    if (!text.isEmpty()) setText(txt);

    /** Definindo ícone */
    QString theme = JsonTools::readJson("theme");
    if (Utils::setIconTheme(theme, ico) == nullptr)
        setIcon(QIcon::fromTheme(Utils::defaultIcon(ico)));
    else setIcon(QIcon(Utils::setIconTheme(theme, ico)));

    /** ToolTip */
    if (tooltip.isEmpty()) {
        std::string upper = ico.toStdString();
        std::transform(upper.begin(), upper.end(), upper.begin(), ::tolower);
        upper[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(upper[0]))); /** C++ 17 */
        setToolTip(QString::fromStdString(upper));
    } else if (QString::compare(tooltip, NOTOOLTIP, Qt::CaseSensitive) != 0)
        setToolTip(tooltip);

    if (fixed) connect(this, SIGNAL(pressed()), SLOT(onEffect()));
}


/** Destrutor */
Button::~Button() = default;


/**********************************************************************************************************************/


/** Iniciando o efeito do botão */
void Button::onEffect() {
    qDebug("%s(%sDEBUG%s):%s Clique no botão %s ...\033[0m", GRE, RED, GRE, VIO, qUtf8Printable(ico));
    setIconSize(QSize(num - 2, num - 2));
    QTimer::singleShot(100, this, SLOT(unEffect()));
}


/** Finalizando o efeito do botão */
void Button::unEffect() {
    setIconSize(QSize(num + 2, num + 2));
}


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
