#include <QContextMenuEvent>
#include <QDebug>
#include <QEvent>
#include <QTimer>
#include <QWidget>

#include "EventFilter.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Construtor do filtro de eventos */
EventFilter::EventFilter(QWidget *parent, UITYPE i) : QObject(parent), option(i) {}


/** Destrutor */
EventFilter::~EventFilter() = default;


/** Funções auxiliares */
void EventFilter::setMove(bool var)  { moving = var; }
void EventFilter::setFixed(bool var) { fixed = var;  }
void EventFilter::setSett(bool var)  { sett = var;   }


/**********************************************************************************************************************/


/** Filtro de eventos que irá mapear tudo o que tem de direito */
bool EventFilter::eventFilter(QObject *object, QEvent *event) {
    Q_UNUSED(object)

    if (option == General) {
        /** O widget filho abre mais rápido que o pai, por isso o delay */
        if (!first) QTimer::singleShot(1200, [this](){ start = true; first = true; });

        /** Método para criar as teclas de atalho */
        if (event->type() == QEvent::KeyPress) {
            auto *ke = dynamic_cast<QKeyEvent *>(event);
            int key = ke->key();
            Qt::KeyboardModifiers modifiers = ke->modifiers();

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
                    if (modifiers == (CTRL|ALT)) emit emitReplayOne();
                    break;
                case Qt::Key_S:
                    if (modifiers == ALT) {
                        emit emitSettings();
                        emit emitLeave();
                    }
                    break;
                case Qt::Key_MediaPlay:
                    emit emitPlay();
                    break;
                case Qt::Key_MediaNext:
                    emit emitNext();
                    break;
                case Qt::Key_MediaPrevious:
                    emit emitPrevious();
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
            arrowMouse();
            emit emitMouseMove();
            moving = true;
        }

        /** Aproveitando o evento do tooltip para usar como mapeamento de mouse parado */
        if (event->type() == QEvent::ToolTip) {
            if (!fixed) {
                qDebug("%s(%sDEBUG%s):%s Mouse sem Movimentação ...\033[0m", GRE, RED, GRE, YEL);
                emit emitLeave();
            }
            moving = false;
        }

        /** Ação após a destruição do menu de contexto */
        if (event->type() == QEvent::ChildRemoved && contextmenu) {
            qDebug("%s(%sDEBUG%s):%s Finalizando o Menu de Contexto ...\033[0m", GRE, RED, GRE, CYA);
            if (sett || fixed) arrowMouse();
            else blankMouse();
            contextmenu = false;
        }

        /** Emissão ao pressionar o mouse */
        if (event->type() == QEvent::MouseButtonPress) {
            auto *me = dynamic_cast<QMouseEvent*>(event);
            Qt::MouseButton bt = me->button();
            if (bt == Qt::LeftButton) emit emitMousePress();
        }

        /** Emissão ao soltar o botão do mouse */
        if (event->type() == QEvent::MouseButtonRelease) emit emitMouseRelease();

        /** Mapeando duplo clique para o fullscreen */
        if (event->type() == QEvent::MouseButtonDblClick) emit emitDoubleClick();

        /** Emissão para ativar o menu de contexto */
        if (event->type() == QEvent::ContextMenu) {
            contextmenu = true;
            if (!fixed) emit emitLeave();
            auto *e = dynamic_cast<QContextMenuEvent *>(event);
            emit customContextMenuRequested(e->pos());
        }
    }

    /** Emissão para os controles */
    if (option == Control && event->type() == QEvent::Leave) emit emitLeave();

    /** Emissão para outros widgets */
    if (option == Other) {
        if (event->type() == QEvent::Enter) emit emitEnter();
        if (event->type() == QEvent::Leave) emit emitLeave();
    }

    return false;
}
