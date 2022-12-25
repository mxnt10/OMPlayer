#ifndef OMPLAYER_CMENU_H
#define OMPLAYER_CMENU_H

#include <QMenu>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

class CMenu : public QMenu {
Q_OBJECT
public:
    explicit CMenu(const QString& title, QWidget *parent = nullptr);
    ~CMenu() override;

protected:
    void showEvent(QShowEvent *event) override;

private:
    QGraphicsOpacityEffect *effect{};
    QPropertyAnimation *animation{};
};

#endif // OMPLAYER_CMENU_H
