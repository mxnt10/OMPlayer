#ifndef OMPLAYER_BUTTON_H
#define OMPLAYER_BUTTON_H

#include <QPushButton>

class Button : public QPushButton {
Q_OBJECT

public:
    enum BUTTONTYPE{ Default = 0, Radio = 1, Tag = 2 };
    explicit Button(Button::BUTTONTYPE btn, const QString &icon, int size = 0, const QString &text = nullptr);
    ~Button() override;

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    int num{0};
    Button::BUTTONTYPE type{Button::Default};
    QString ico{};
    QString txt{};

Q_SIGNALS:
    void emitEnter();
};

#endif //OMPLAYER_BUTTON_H
