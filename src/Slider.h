#ifndef OMPLAYER_SLIDER_H
#define OMPLAYER_SLIDER_H

#include <QSlider>


/**
 * Classe Slider, para personalização do QSlider do Programa.
 **********************************************************************************************************************/


class Slider : public QSlider {
Q_OBJECT
public:
    explicit Slider(QWidget *parent = nullptr, bool disable = false, int size = (-1), int maximum = (-1));
    ~Slider() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    [[nodiscard]] inline int pick(const QPoint &pt) const;
    [[nodiscard]] int pixelPosToRangeValue(int pos) const;

signals:
    void emitEnter();
    void emitLeave();
    void onHover(int pos, int value);
};

#endif //OMPLAYER_SLIDER_H
