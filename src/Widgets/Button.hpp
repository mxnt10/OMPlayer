#ifndef OMPLAYER_BUTTON_HPP
#define OMPLAYER_BUTTON_HPP

#include <QThread>
#include <QPushButton>
#include "LoopButton.hpp"

class Button : public QPushButton {
Q_OBJECT

public:
    enum ButtonType{ NormalBtn = 0, LoopBtn = 1, DoubleBtn = 2 };
    explicit Button(Button::ButtonType btn, int size = 0, const QString &ico = nullptr, const QString &ico2 = nullptr);
    ~Button() override;

    void secondIcon(const QString &ico, const QString &ico2);

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

Q_SIGNALS:
    void longPress();
    void loopPress();
    void customPress();

private:
    int num{0};
    bool emitted{false}, prevent{true}, block{false};
    Button::ButtonType type{Button::NormalBtn};
    QString icon{};
    QIcon icon1{}, icon2{};
    QString txt{};

    /** Loop do botão pressionado em multithread */
    QThread *thread;
    Loop *loop;
};

#endif //OMPLAYER_BUTTON_HPP
