#ifndef OMPLAYER_SLIDER_HPP
#define OMPLAYER_SLIDER_HPP

#define null (-1)

#include <QSlider>

class Slider : public QSlider {
Q_OBJECT

public:
    explicit Slider(QWidget *parent = nullptr, bool disable = false,
                    int w = null, int h = null, int max = null,
                    const QString &style = nullptr, const QString &hover = nullptr);
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
    void getValue(int value) { val = value; };

private:
    [[nodiscard]] inline int pick(const QPoint &pt) const;
    [[nodiscard]] int pixelPosToRangeValue(int pos) const;
    int val{0};
    QString stl{}, hvr{};
};

#endif //OMPLAYER_SLIDER_HPP
