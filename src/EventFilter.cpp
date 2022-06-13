#include <QContextMenuEvent>
#include <QDebug>
#include <QEvent>
#include <QTimer>
#include <QWidget>

#include "Defines.h"
#include "EventFilter.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Construtor do filtro de eventos */
EventFilter::EventFilter(QWidget *parent, int i) : QObject(parent) {
    num = i;
}


/** Destrutor */
EventFilter::~EventFilter() = default;


/**********************************************************************************************************************/


/** Método para alterar o valor da váriável moving */
void EventFilter::setMove(bool var) {
    moving = var;
}


/** Método para alterar o valor da váriável fixed */
void EventFilter::setFixed(bool var) {
    fixed = var;
}


/** Método para alterar o valor da váriável sett */
void EventFilter::setSett(bool var) {
    sett = var;
}


/** Método auxiliar para o delay para exibição do widget flutuante */
void EventFilter::startShow(){
    start = true;
}


/** Filtro de eventos que irá mapear tudo o que tem de direito */
bool EventFilter::eventFilter(QObject *object, QEvent *event) {
    Q_UNUSED(object);

    if (num == 1) {
        /** O widget filho abre mais rápido que o pai, por isso esse delay */
        if (!first) {
            QTimer::singleShot(1200, this, SLOT(startShow()));
            first = true;
        }

        /** Método para criar as teclas de atalho */
        if (event->type() == QEvent::KeyPress) {
            auto *key_event = dynamic_cast<QKeyEvent *>(event);
            int key = key_event->key();
            Qt::KeyboardModifiers modifiers = key_event->modifiers();

            switch (key) {
                case Qt::Key_A:
                case Qt::Key_O:
                    if (modifiers == CTRL) emit emitOpen();
                    break;
                case Qt::Key_H:
                    if (modifiers == CTRL) emit emitShuffle();
                    break;
                case Qt::Key_T:
                    if (modifiers == CTRL) emit emitReplay();
                    break;
                case Qt::Key_S:
                    if (modifiers == ALT) {
                        emit emitSettings();
                        emit emitLeave();
                    }
                    break;
                case ENTER:
                    if (modifiers == ALT) emit emitDoubleClick();
                    break;
                case ESC:
                    emit emitEsc();
                    break;
                default:
                    break;
            }
            return false;
        }

        /** Mapeando o movimento do mouse */
        if (event->type() == QEvent::MouseMove && !moving && start) {
            qDebug("%s(%sDEBUG%s):%s Mouse com Movimentação ...\033[0m", GRE, RED, GRE, DGR);
            Utils::arrowMouse();
            emit emitMouseMove();
            moving = true;
        }

        /** Aproveitando o evento do tooltip para usar como mapeamento de mouse parado */
        if (event->type() == QEvent::ToolTip) {
            qDebug("%s(%sDEBUG%s):%s Mouse sem Movimentação ...\033[0m", GRE, RED, GRE, YEL);
            if (!fixed) {
                Utils::blankMouse();
                emit emitLeave();
            }
            moving = false;
        }

        /** Ação após a destruição do menu de contexto */
        if (event->type() == QEvent::ChildRemoved && contextmenu) {
            qDebug("%s(%sDEBUG%s):%s Finalizando o Menu de Contexto ...\033[0m", GRE, RED, GRE, CYA);
            if (sett)
                Utils::arrowMouse();
            else
                Utils::blankMouse();
            contextmenu = false;
        }

        /** Emissão ao pressionar o mouse */
        if (event->type() == QEvent::MouseButtonPress) {
            emit emitMousePress();
        }

        /** Emissão ao soltar o botão do mouse */
        if (event->type() == QEvent::MouseButtonRelease) {
            emit emitMouseRelease();
        }

        /** Emissão para ativar o menu de contexto */
        if (event->type() == QEvent::ContextMenu) {
            contextmenu = true;
            emit emitLeave();
            auto *e = dynamic_cast<QContextMenuEvent *>(event);
            emit customContextMenuRequested(e->pos());
        }

        /** Mapeando duplo clique para o fullscreen */
        if (event->type() == QEvent::MouseButtonDblClick) {
            emit emitDoubleClick();
        }
    }

    /** Emissão para os controles */
    if (num == 2) {
        if (event->type() == QEvent::Enter)
            qDebug("%s(%sDEBUG%s):%s Ponteito do Mouse Sobre a Interface Flutuante ...\033[0m", GRE, RED, GRE, DGR);

        /** Mapeador para executar ações quando o ponteiro do mouse se encontra fora da interface */
        if (event->type() == QEvent::Leave) {
            qDebug("%s(%sDEBUG%s):%s Ponteito do Mouse Fora da Interface Flutuante ...\033[0m", GRE, RED, GRE, YEL);
            emit emitLeave();
        }
    }

    /** Emissão para QWidget */
    if (num == 3) {
        if (event->type() == QEvent::Enter)
            emit emitEnter();
    }

    return false;
}
