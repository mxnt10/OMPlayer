#include <QApplication>
#include <QMouseEvent>
#include <QSlider>
#include <QStyleOption>

#include "Slider.h"
#include "Utils.h"

using namespace Qt;
using QStyle::CC_Slider;
using QStyle::SC_SliderGroove;
using QStyle::SC_SliderHandle;


/**********************************************************************************************************************/


/** Construtor principal do Slider. */
Slider::Slider(QWidget *parent) : QSlider(parent) {
    setOrientation(Horizontal);
    setFocusPolicy(NoFocus);
    setMouseTracking(true);
    setStyleSheet(Utils::setStyle("progressbar"));
}


/** Destrutor */
Slider::~Slider() = default;


/**********************************************************************************************************************/


/** Função para retornar um valor de x e y para pixelPosToRangeValue */
inline int Slider::pick(const QPoint &point) const {
    return orientation() == Horizontal ? point.x() : point.y();
}


/** Função para setar o valor do Slider */
int Slider::pixelPosToRangeValue(int pos) const {
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect gr = style()->subControlRect(CC_Slider, &opt, SC_SliderGroove, this);
    QRect sr = style()->subControlRect(CC_Slider, &opt, SC_SliderHandle, this);
    int sliderMin, sliderMax, sliderLength;

    if (orientation() == Horizontal) {
        sliderLength = sr.width();
        sliderMin = gr.x();
        sliderMax = gr.right() - sliderLength + 1;
    } else {
        sliderLength = sr.height(); /** Suporte a orientação vertical */
        sliderMin = gr.y();
        sliderMax = gr.bottom() - sliderLength + 1;
    }

    return QStyle::sliderValueFromPosition(
            minimum(), maximum(), pos - sliderMin, sliderMax - sliderMin, opt.upsideDown);
}


/**********************************************************************************************************************/


/** Evento que possibilita habilitar o recurso no Slider para pular a execução do arquivo
 * multimídia ao clicar no QSlider */
void Slider::mousePressEvent(QMouseEvent *event) {
    if (event->button() == LeftButton) { /** Botão esquerdo do Mouse */
        QStyleOptionSlider opt;
        initStyleOption(&opt);

        const QRect sliderRect = style()->subControlRect(CC_Slider, &opt, SC_SliderHandle, this);
        const QPoint center = sliderRect.center() - sliderRect.topLeft();

        if (!sliderRect.contains(event->pos())) {
            event->accept();
            setSliderPosition(pixelPosToRangeValue(pick(event->pos() - center)));
            triggerAction(SliderMove);
            setRepeatAction(SliderNoAction);

            /** Emições que são feitas para habilitar o recurso de mudar a posição do QSlider clicando em
             * qualquer lugar do QSlider. */
            emit sliderMoved(pixelPosToRangeValue(pick(event->pos() - center)));
            emit sliderPressed();
        } else {
            QSlider::mousePressEvent(event);
        }
    } else {
        QSlider::mousePressEvent(event);
    }
}


/** Evento para mapear a posição do mouse no slider */
void Slider::mouseMoveEvent(QMouseEvent *event) {
    const int o = style()->pixelMetric(QStyle::PM_SliderLength ) - 1;
    int v = QStyle::sliderValueFromPosition(minimum(), maximum(), event->pos().x()-o/2, width()-o, false);
    emit onHover(event->x(), v);
    QSlider::mouseMoveEvent(event);
}


/** Ação ao posicionar o mouse sobre o botão */
void Slider::enterEvent(QEvent *event) {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[35m Mouse posicionado na barra de reprodução ...\033[0m");
    emit emitEnter();
    QSlider::enterEvent(event);
}


/** Ação ao desposicionar o mouse sobre o botão */
void Slider::leaveEvent(QEvent *event) {
    emit emitLeave();
    QSlider::leaveEvent(event);
}
