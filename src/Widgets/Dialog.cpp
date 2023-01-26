#include <QGuiApplication>
#include <QKeyEvent>
#include <QScreen>
#include <Utils>
#include "Dialog.hpp"


/**********************************************************************************************************************/


/** Construtor */
Dialog::Dialog(QWidget *parent) : QDialog(parent) {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setModal(true);
    setFocus();

    /** Efeito de transparência funcional. O setWindowOpacity() não rola. */
    effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(0);
    setGraphicsEffect(effect);
    animation = new QPropertyAnimation(effect, "opacity");

}


/** Destrutor */
Dialog::~Dialog() = default;


/**********************************************************************************************************************/


/** Emissão para fechar a janela */
void Dialog::onClose() {
    qDebug("%s(%sSettings%s)%s::%sFechando um diálogo ...\033[0m", GRE, RED, GRE, RED, CYA);
    onclose = true;
    Utils::fadeDiag(animation, 1, 0);
    connect(animation, &QPropertyAnimation::finished, [this](){
        if (!onclose) return;
        Q_EMIT emitclose();
        this->close();
    });
}


/**********************************************************************************************************************/


/** Apenas para redefinir a variável onclose */
void Dialog::showEvent(QShowEvent *event) {
    onclose = false;
    Utils::fadeDiag(animation, 0, 1);
    QDialog::showEvent(event);
}


/** Corrigindo fechamento do diálogo com Escape */
void Dialog::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Escape) {
        onClose();
        return;
    }
    QDialog::keyPressEvent(event);
}


/** Prevenindo fechamento sem onClose() */
void Dialog::closeEvent(QCloseEvent *event) {
    if (!onclose) {
        event->ignore();
        onClose();
        return;
    } else event->accept();
}
