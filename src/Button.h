#ifndef OMPLAYER_BUTTON_H
#define OMPLAYER_BUTTON_H

#include <QPushButton>

class Button : public QPushButton {
Q_OBJECT
public:
    enum ButtonType{ button = 1, radio = 2, tag = 3};
    explicit Button(ButtonType btn, const QString &icon, int size = 0, const QString &tooltip = nullptr, const QString &text = nullptr);
    ~Button() override;

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    int num{0};
    ButtonType type{};
    QString ico{};
    QString txt{};

signals:
    void emitEnter();
};

#endif //OMPLAYER_BUTTON_H
