#include <QApplication>
#include <QMouseEvent>
#include <QStyleOption>
#include <Utils>

#include "Slider.hpp"


/**********************************************************************************************************************/


/** Construtor principal do Slider. */
Slider::Slider(QWidget *parent, bool disable, int w, int h, int max, const QString &style, const QString &hover) :
    QSlider(parent) {
    setOrientation(Qt::Horizontal);
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true);
    setTracking(true);
    setDisabled(disable);
    setStyleSheet(Utils::setStyle(style));
    connect(this, &Slider::valueChanged, this, &Slider::getValue);
    stl = style;
    hvr = hover;

    if (w > null) setFixedWidth(w);
    if (h > null) setFixedHeight(h);
    if (max > null) setMaximum(max);
}


/** Destrutor */
Slider::~Slider() = default;


/**********************************************************************************************************************/


/** Função para retornar um valor de x e y para pixelPosToRangeValue */
inline int Slider::pick(const QPoint &point) const {
    return orientation() == Qt::Horizontal ? point.x() : point.y();
}


/** Função para setar o valor do Slider */
int Slider::pixelPosToRangeValue(int pos) const {
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect gr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
    QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    int sliderMin, sliderMax, sliderLength;

    if (orientation() == Qt::Horizontal) {
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
    if (event->button() == Qt::LeftButton) { /** Botão esquerdo do Mouse */
        QStyleOptionSlider opt;
        initStyleOption(&opt);

        const QRect sliderRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
        const QPoint center = sliderRect.center() - sliderRect.topLeft();

        if (!sliderRect.contains(event->pos())) {
            event->accept();
            int value = pixelPosToRangeValue(pick(event->pos() - center));
            setSliderPosition(value);
            triggerAction(SliderMove);
            setRepeatAction(SliderNoAction);

            /** Emições que são feitas para habilitar o recurso de mudar a posição do QSlider clicando em
             * qualquer lugar do QSlider. */
            emit sliderMoved(value);
            emit sliderPressed();
        } else QSlider::mousePressEvent(event);
    } else QSlider::mousePressEvent(event);
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
    qDebug("%s(%sSlider%s)%s::%sMouse posicionado no %s ...\033[0m", GRE, RED, GRE, RED, VIO, STR(stl));
    if (!hvr.isEmpty()) setStyleSheet(Utils::setStyle(hvr));
    emit emitEnter();
    QSlider::enterEvent(event);
}


/** Ação ao desposicionar o mouse sobre o botão */
void Slider::leaveEvent(QEvent *event) {
    if (!hvr.isEmpty()) setStyleSheet(Utils::setStyle(stl));
    emit emitLeave();
    QSlider::leaveEvent(event);
}


/** Evento que mapeia a movimentação do mouse */
void Slider::wheelEvent(QWheelEvent *event) {
    if (!hvr.isEmpty()) {
        if (event->angleDelta().y() > 0) emit sliderMoved(val + 5);
        else emit sliderMoved(val - 5);
    } else emit sliderMoved(val);
    QSlider::wheelEvent(event);
}
