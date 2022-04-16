#include <QtCore>
#include <QFile>
#include <QMouseEvent>
#include <QSlider>
#include <QStyleOptionSlider>

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
    setFocusPolicy(Qt::NoFocus);
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


/** Ação ao posicionar o mouse sobre o botão */
void Slider::enterEvent(QEvent *event) {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[35m Mouse posicionado na barra de reprodução ...\033[0m");
    emit emitEnter();
}


/** Ação ao desposicionar o mouse sobre o botão */
void Slider::leaveEvent(QEvent *event) {
    emit emitLeave();
}
