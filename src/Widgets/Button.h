#ifndef OMPLAYER_BUTTON_H
#define OMPLAYER_BUTTON_H

#include <QThread>
#include <QPushButton>
#include "LoopButton.h"

class Button : public QPushButton {
Q_OBJECT

public:
    enum BUTTONTYPE{ Default = 0, Tag = 1 , LoopBtn = 2, DoubleBtn = 3};
    explicit Button(Button::BUTTONTYPE btn, int size = 0, const QString &ico = nullptr, const QString &ico2 = nullptr);
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
    Button::BUTTONTYPE type{Button::Default};
    QString icon{};
    QIcon icon1{}, icon2{};
    QString txt{};

    /** Loop do botão pressionado em multithread */
    QThread *thread;
    Loop *loop;
};

#endif //OMPLAYER_BUTTON_H
