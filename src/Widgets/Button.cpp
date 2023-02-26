#include <QPainter>
#include <QTimer>
#include <string>
#include <Utils>

#include "Button.hpp"


/**********************************************************************************************************************/


/** Construtor que define a classe dos botões do reprodutor */
Button::Button(Button::ButtonType btn, int size, const QString &ico, const QString &ico2) : num(size), type(btn) {
    setStyleSheet("QPushButton {border: 0; background-color: transparent;}");
    setIconSize(QSize(num, num));
    setFocusPolicy(Qt::NoFocus);
    setFixedSize(num, num);
    setMouseTracking(true);
    secondIcon(ico, ico2);

    /** Definindo thread para o loop do clique */
    if (type == Button::LoopBtn) {
        thread = new QThread();
        loop = new Loop();
        loop->moveToThread(thread);

        connect(thread, &QThread::started, loop, &Loop::doLoop);
        connect(loop, &Loop::looping, [this]() { Q_EMIT loopPress(); });
        connect(loop, &Loop::workRequested, [this]() { thread->start(); });
        connect(loop, &Loop::finished, [this]() { thread->quit(); });
    }

    /** Apenas para o Debug */
    std::string upper = ico.toStdString();
    std::transform(upper.begin(), upper.end(), upper.begin(), ::tolower);
    upper[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(upper[0]))); /** C++ 17 */
    icon = QString::fromStdString(upper);
}


/** Destrutor */
Button::~Button() = default;


/**********************************************************************************************************************/


/** Setando o ícones primário e secundário */
void Button::secondIcon(const QString &ico, const QString &ico2) {
    QString theme = JsonTools::stringJson("theme");
    if (Utils::setIconTheme(theme, ico) == nullptr)
        setIcon(QIcon::fromTheme(Utils::defaultIcon(ico)));
    else {
        QPixmap pixmap{Utils::setIconTheme(theme, ico)};

        QPainter p;
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
        p.setRenderHint(QPainter::LosslessImageRendering, true);
        p.drawPixmap(QRect(100, 100, num, num), pixmap, pixmap.rect());

        QIcon img;
        img.addPixmap(pixmap);
        img.paint(&p, QRect(100, 100, num, num));
        setIcon(img);
        icon1 = img;
    }

    if (!ico2.isEmpty()) {
        if (Utils::setIconTheme(theme, ico2) == nullptr)
            icon2 = QIcon::fromTheme(Utils::defaultIcon(ico2));
        else {
            QPixmap pixmap{Utils::setIconTheme(theme, ico2)};

            QPainter p;
            p.setRenderHint(QPainter::Antialiasing, true);
            p.setRenderHint(QPainter::SmoothPixmapTransform, true);
            p.setRenderHint(QPainter::LosslessImageRendering, true);
            p.drawPixmap(QRect(100, 100, num, num), pixmap, pixmap.rect());

            QIcon img;
            img.addPixmap(pixmap);
            img.paint(&p, QRect(100, 100, num, num));
            icon2 = img;
        }
    }
}


/**********************************************************************************************************************/


/** Ação ao posicionar o mouse sobre o botão */
void Button::enterEvent(QEvent *event) {
    qDebug("%s(%sButton%s)%s::%sMouse posicionado no botão %s ...\033[0m", GRE, RED, GRE, RED, VIO, STR(icon));
    setIconSize(QSize(num + 2, num + 2));
    QPushButton::enterEvent(event);
}


/** Ação ao desposicionar o mouse sobre o botão */
void Button::leaveEvent(QEvent *event) {
    setIconSize(QSize(num, num));
    QPushButton::leaveEvent(event);
}


/** Iniciando o efeito do botão */
void Button::mousePressEvent(QMouseEvent *event) {
    if (!prevent) return;
    qDebug("%s(%sButton%s)%s::%sPressioando o botão %s ...\033[0m", GRE, RED, GRE, RED, VIO, STR(icon));
    setIconSize(QSize(num - 2, num - 2));

    if (type == Button::LoopBtn || type == Button::DoubleBtn) {
        block = emitted = false;
        QTimer::singleShot(300, [this]() {
            if (!block) {
                setIcon(icon2);
                if (type == Button::LoopBtn) loop->requestWork();
                else {
                    Q_EMIT longPress();
                    emitted = true;
                }
            }
        });
    }
    QPushButton::mousePressEvent(event);
}


/** Finalizando o efeito do botão */
void Button::mouseReleaseEvent(QMouseEvent *event) {
    if (!prevent) return;
    setIconSize(QSize(num + 2, num + 2));

    if (type == Button::LoopBtn || type == Button::DoubleBtn) {
        prevent = false;
        QTimer::singleShot(1500, [this]() { prevent = true; });

        block = true;
        if (type == Button::LoopBtn) {
            if (thread->isRunning()) {
                loop->End();
                setIcon(icon1);
                return;
            } else Q_EMIT customPress();
        } else if (emitted && type == Button::DoubleBtn) return;
        else Q_EMIT customPress();
    }
    QPushButton::mouseReleaseEvent(event);
}
