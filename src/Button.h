#ifndef OMPLAYER_BUTTON_H
#define OMPLAYER_BUTTON_H

#include <QPushButton>

using namespace std;

QT_BEGIN_NAMESPACE
class QPushButton;
class QString;
QT_END_NAMESPACE


/**
 * Classe Button, para inserir efeito nos Botões.
 **********************************************************************************************************************/


class Button : public QPushButton {
Q_OBJECT
public:
    explicit Button(const string &icon, int num);

    ~Button() override;

private Q_SLOTS:

    void onEffect();

    void unEffect();

protected:
    void enterEvent(QEvent *event) override;

    void leaveEvent(QEvent *event) override;

private:
    int num;
    QString ico;
    const char *theme;

signals:

    void emitEnter();
};

#endif //OMPLAYER_BUTTON_H
