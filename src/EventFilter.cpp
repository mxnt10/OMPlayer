#include <QContextMenuEvent>
#include <QEvent>
#include <QTimer>
#include <Utils>

#include "EventFilter.hpp"


/**********************************************************************************************************************/


/** Construtor e destrutor do filtro de eventos */
EventFilter::EventFilter(QWidget *parent, EventFilter::Type i) : QObject(parent), option(i) {}
EventFilter::~EventFilter() = default;


/** Funções auxiliares */
void EventFilter::setMove(bool var)  { moving = var; }
void EventFilter::setFixed(bool var) { fixed = var;  }
void EventFilter::setSett(bool var)  { sett = var;   }


/**********************************************************************************************************************/


/** Filtro de eventos que irá mapear tudo o que tem de direito */
bool EventFilter::eventFilter(QObject *object, QEvent *event) {
    Q_UNUSED(object)

    if (option == EventFilter::NormalEvent) {
        /** Método para criar as teclas de atalho */
        if (event->type() == QEvent::KeyPress) {
            auto *ke = dynamic_cast<QKeyEvent *>(event);
            int key = ke->key();
            Qt::KeyboardModifiers modifiers = ke->modifiers();

            switch (key) {
                case Qt::Key_O:
                    if (modifiers == CTRL) Q_EMIT emitOpen();
                    break;
                case Qt::Key_H:
                    if (modifiers == CTRL) Q_EMIT emitShuffle();
                    break;
                case Qt::Key_T:
                    if (modifiers == CTRL) Q_EMIT emitReplay();
                    if (modifiers == (CTRL|ALT)) Q_EMIT emitReplayOne();
                    break;
                case Qt::Key_S:
                    if (modifiers == ALT) {
                        Q_EMIT emitSettings();
                        Q_EMIT emitLeave();
                    }
                    break;
                case Qt::Key_MediaPlay:
                    Q_EMIT emitPlay();
                    break;
                case Qt::Key_MediaNext:
                    Q_EMIT emitNext();
                    break;
                case Qt::Key_MediaPrevious:
                    Q_EMIT emitPrevious();
                    break;
                case ENTER:
                    if (modifiers == ALT) Q_EMIT emitDoubleClick();
                    break;
                case ESC:
                    Q_EMIT emitEsc();
                    break;
                default:
                    break;
            }
            return false;
        }

        /** Mapeando o movimento do mouse */
        if (event->type() == QEvent::MouseMove && !moving) {
            qDebug("%s(%sEventFilter%s)%s::%sMouse com Movimentação ...\033[0m", GRE, RED, GRE, RED, DGR);
            arrowMouse();
            Q_EMIT emitMouseMove();
            moving = true;
        }

        /** Aproveitando o evento do tooltip para usar como mapeamento de mouse parado */
        if (event->type() == QEvent::ToolTip) {
            if (!fixed) {
                qDebug("%s(%sEventFilter%s)%s::%sMouse sem Movimentação ...\033[0m", GRE, RED, GRE, RED, YEL);
                Q_EMIT emitLeave();
            }
            moving = false;
        }

        /** Ação após a destruição do menu de contexto */
        if (event->type() == QEvent::ChildRemoved && contextmenu) {
            qDebug("%s(%sEventFilter%s)%s::%sFinalizando o Menu de Contexto ...\033[0m", GRE, RED, GRE, RED, CYA);
            if (sett || fixed) arrowMouse();
            else blankMouse();
            contextmenu = false;
        }

        /** Emissão ao pressionar o mouse */
        if (event->type() == QEvent::MouseButtonPress) {
            auto *me = dynamic_cast<QMouseEvent*>(event);
            Qt::MouseButton bt = me->button();
            if (bt == Qt::LeftButton) Q_EMIT emitMousePress();
        }

        /** Emissão ao soltar o botão do mouse */
        if (event->type() == QEvent::MouseButtonRelease) Q_EMIT emitMouseRelease();

        /** Mapeando duplo clique para o fullscreen */
        if (event->type() == QEvent::MouseButtonDblClick) Q_EMIT emitDoubleClick();

        /** Emissão para ativar o menu de contexto */
        if (event->type() == QEvent::ContextMenu) {
            contextmenu = true;
            auto *e = dynamic_cast<QContextMenuEvent *>(event);
            Q_EMIT customContextMenuRequested(e->pos());
        }
    }

    /** Emissão para os controles e demais */
    if (option == EventFilter::ControlEvent) {
        if (event->type() == QEvent::Enter) Q_EMIT emitEnter();
        if (event->type() == QEvent::Leave) Q_EMIT emitLeave();
    }

    return false;
}
