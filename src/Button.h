#ifndef OMPLAYER_BUTTON_H
#define OMPLAYER_BUTTON_H

#define NOTOOLTIP "NONE"

#include <QPushButton>

/**
 * Classe Button, para inserir efeito nos Botões.
 **********************************************************************************************************************/


class Button : public QPushButton {
Q_OBJECT
public:
    explicit Button(const QString &icon, int size = 0, const QString &tooltip = nullptr, const QString &text = nullptr, bool fixed = true);
    ~Button() override;

private Q_SLOTS:
    void onEffect();
    void unEffect();

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    int num;
    bool fix;
    QString ico;
    const char *theme;

signals:
    void emitEnter();
};

#endif //OMPLAYER_BUTTON_H
