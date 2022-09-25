#ifndef OMPLAYER_SLIDER_H
#define OMPLAYER_SLIDER_H

#include <QSlider>

class Slider : public QSlider {
Q_OBJECT
public:
    explicit Slider(QWidget *parent = nullptr,
                    bool disable = false,
                    int wsize = (-1),
                    int hsize = (-1),
                    int maximum = (-1),
                    const QString &style = "slider");
    ~Slider() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

Q_SIGNALS:
    void emitEnter();
    void emitLeave();
    void onHover(int pos, int value);

private Q_SLOTS:
    void getValue(int value);

private:
    [[nodiscard]] inline int pick(const QPoint &pt) const;
    [[nodiscard]] int pixelPosToRangeValue(int pos) const;
    int val{0};
    QString stl{};
};

#endif //OMPLAYER_SLIDER_H
