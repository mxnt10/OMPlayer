#include <QApplication>
#include <QMouseEvent>
#include <QStyleOption>

#include "Slider.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Construtor principal do Slider. */
Slider::Slider(QWidget *parent, bool disable, int wsize, int hsize, int maximum, const QString &style) :
    QSlider(parent) {
    setOrientation(Qt::Horizontal);
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true);
    setTracking(true);
    setDisabled(disable);
    setStyleSheet(Utils::setStyle(style));
    connect(this, SIGNAL(valueChanged(int)), SLOT(getValue(int)));
    stl = style;

    if (wsize > (-1)) setFixedWidth(wsize);
    if (hsize > (-1)) setFixedHeight(hsize);
    if (maximum > (-1)) setMaximum(maximum);
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


/** Função para coletar o valor atual do slider */
void Slider::getValue(int value) {
    val = value;
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
        } else
            QSlider::mousePressEvent(event);
    } else
        QSlider::mousePressEvent(event);
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
    qDebug("%s(%sDEBUG%s):%s Mouse posicionado no %s ...\033[0m", GRE, RED, GRE, VIO, qUtf8Printable(stl));
    emit emitEnter();
    QSlider::enterEvent(event);
}


/** Ação ao desposicionar o mouse sobre o botão */
void Slider::leaveEvent(QEvent *event) {
    emit emitLeave();
    QSlider::leaveEvent(event);
}


/** Evento que mapeia a movimentação do mouse */
void Slider::wheelEvent(QWheelEvent *event) {
    emit sliderMoved(val);
    QSlider::wheelEvent(event);
}
