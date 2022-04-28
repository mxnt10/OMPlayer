#include <QDebug>
#include <QPushButton>
#include <QTimer>
#include <string>

#include "Button.h"
#include "Defines.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Construtor que define a classe dos botões do reprodutor */
Button::Button(const string& icon, int size) {
    theme = "circle";
    num = size;
    ico = QString().fromStdString(icon); /** Apenas para o debug */
    setFixedSize(num, num);
    setIconSize(QSize(num, num));

    /** Definindo ícone */
    if (Utils::setIconTheme(theme, icon) == nullptr)
        setIcon(QIcon::fromTheme(Utils::defaultIcon(icon)));
    else setIcon(QIcon(Utils::setIconTheme(theme, icon)));

    setFocusPolicy(Qt::NoFocus);
    setStyleSheet("QPushButton { border: 0; }");
    connect(this, PRESSED, SLOT(onEffect()));
}


/** Destrutor */
Button::~Button() = default;


/**********************************************************************************************************************/


/** Iniciando o efeito do botão */
void Button::onEffect() {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[35m Clique no botão %s ...\033[0m", ico.toStdString().c_str());
    setIconSize(QSize(num - 2, num - 2));
    QTimer::singleShot(100, this, SLOT(unEffect()));
}


/** Finalizando o efeito do botão */
void Button::unEffect() {
    setIconSize(QSize(num + 2, num + 2));
}


/**********************************************************************************************************************/


/** Ação ao posicionar o mouse sobre o botão */
void Button::enterEvent( QEvent *event ) {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[35m Mouse posicionado no botão %s ...\033[0m", ico.toStdString().c_str());
    setIconSize(QSize(num + 2, num + 2));
    emit emitEnter();
    QPushButton::enterEvent(event);
}


/** Ação ao desposicionar o mouse sobre o botão */
void Button::leaveEvent( QEvent *event ) {
    setIconSize(QSize(num, num));
    QPushButton::leaveEvent(event);
}

