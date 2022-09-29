#include <QPainter>

#include <string>

#include "Button.h"
#include "JsonTools.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Construtor que define a classe dos botões do reprodutor */
Button::Button(ButtonType btn, const QString &icon, int size, const QString &text) {
    num = size;
    type = btn;
    txt = text;
    setStyleSheet("QPushButton {border: 0; background-color: transparent;}");
    setIconSize(QSize(num, num));
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true);

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
    if (btn != radio) setFixedSize(num, num);
    if (btn == radio) setText(txt);


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

    if (type == button) {
        qDebug("%s(%sDEBUG%s):%s Mouse posicionado no botão %s ...\033[0m", GRE, RED, GRE, VIO, qUtf8Printable(ico));
        setIconSize(QSize(num + 2, num + 2));
    } else if (type == radio) {
        qDebug("%s(%sDEBUG%s):%s Mouse posicionado na seleção %s ...\033[0m", GRE, RED, GRE, VIO, qUtf8Printable(txt));
        setText(" " + txt);
    }
    QPushButton::enterEvent(event);
}


/** Ação ao desposicionar o mouse sobre o botão */
void Button::leaveEvent(QEvent *event) {
    if (type == button) setIconSize(QSize(num, num));
    else if (type == radio) setText(txt);
    QPushButton::leaveEvent(event);
}


/** Iniciando o efeito do botão */
void Button::mousePressEvent(QMouseEvent *event) {
    if (type == button) {
        qDebug("%s(%sDEBUG%s):%s Pressioando o botão %s ...\033[0m", GRE, RED, GRE, VIO, qUtf8Printable(ico));
        setIconSize(QSize(num - 2, num - 2));
    }
    QPushButton::mousePressEvent(event);
}


/** Finalizando o efeito do botão */
void Button::mouseReleaseEvent(QMouseEvent *event) {
    if (type == button) setIconSize(QSize(num + 2, num + 2));
    QPushButton::mouseReleaseEvent(event);
}
