#include <QPainter>
#include <string>
#include <Utils>
#include <utility>

#include "Button.h"


/**********************************************************************************************************************/


/** Construtor que define a classe dos botões do reprodutor */
Button::Button(Button::BUTTONTYPE btn, const QString &icon, int size, const QString &text) : num(size), type(btn) {
    setStyleSheet("QPushButton {border: 0; background-color: transparent;}");
    setIconSize(QSize(num, num));
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true);
    txt = text;

    /** Definindo ícone */
    QString theme = JsonTools::stringJson("theme");
    if (Utils::setIconTheme(theme, icon) == nullptr)
        setIcon(QIcon::fromTheme(Utils::defaultIcon(icon)));
    else {
        QPixmap pixmap{Utils::setIconTheme(theme, icon)};

        QPainter p;
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
        p.setRenderHint(QPainter::LosslessImageRendering, true);
        p.drawPixmap(QRect(100, 100, num, num), pixmap, pixmap.rect());

        QIcon img;
        img.addPixmap(pixmap);
        img.paint(&p, QRect(100, 100, num, num));
        setIcon(img);
    }


    /** Para botões que agem feito radio buttons */
    if (btn != Button::Radio) setFixedSize(num, num);
    if (btn == Button::Radio) setText(txt);


    /** Apenas para o Debug */
    std::string upper = icon.toStdString();
    std::transform(upper.begin(), upper.end(), upper.begin(), ::tolower);
    upper[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(upper[0]))); /** C++ 17 */
    ico = QString::fromStdString(upper);
}


/** Destrutor */
Button::~Button() = default;


/**********************************************************************************************************************/


/** Ação ao posicionar o mouse sobre o botão */
void Button::enterEvent(QEvent *event) {
    emit emitEnter();

    if (type == Button::Default) {
        qDebug("%s(%sButton%s)%s::%sMouse posicionado no botão %s ...\033[0m", GRE, RED, GRE, RED, VIO, STR(ico));
        setIconSize(QSize(num + 2, num + 2));
    } else if (type == Button::Radio) {
        qDebug("%s(%sButton%s)%s::%sMouse posicionado na seleção %s ...\033[0m", GRE, RED, GRE, RED, VIO, STR(txt));
        setText(" " + txt);
    }
    QPushButton::enterEvent(event);
}


/** Ação ao desposicionar o mouse sobre o botão */
void Button::leaveEvent(QEvent *event) {
    if (type == Button::Default) setIconSize(QSize(num, num));
    else if (type == Button::Radio) setText(txt);
    QPushButton::leaveEvent(event);
}


/** Iniciando o efeito do botão */
void Button::mousePressEvent(QMouseEvent *event) {
    if (type == Button::Default) {
        qDebug("%s(%sButton%s)%s::%sPressioando o botão %s ...\033[0m", GRE, RED, GRE, RED, VIO, STR(ico));
        setIconSize(QSize(num - 2, num - 2));
    }
    QPushButton::mousePressEvent(event);
}


/** Finalizando o efeito do botão */
void Button::mouseReleaseEvent(QMouseEvent *event) {
    if (type == Button::Default) setIconSize(QSize(num + 2, num + 2));
    QPushButton::mouseReleaseEvent(event);
}
