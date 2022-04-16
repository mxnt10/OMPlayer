#ifndef OMPLAYER_SLIDER_H
#define OMPLAYER_SLIDER_H

#include <QSlider>


/**
 * Classe Slider, para personalização do QSlider do Programa.
 **********************************************************************************************************************/


class Slider : public QSlider {
Q_OBJECT
public:
    explicit Slider(QWidget *parent = nullptr);

    ~Slider() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void enterEvent(QEvent *event) override;

    void leaveEvent(QEvent *event) override;

private:
    [[nodiscard]] inline int pick(const QPoint &pt) const;

    [[nodiscard]] int pixelPosToRangeValue(int pos) const;

signals:

    void emitEnter();

    void emitLeave();
};

#endif //OMPLAYER_SLIDER_H
