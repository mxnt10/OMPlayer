#ifndef OMPLAYER_CLICKABLEMENU_HPP
#define OMPLAYER_CLICKABLEMENU_HPP

#include <QMenu>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

class ClickableMenu : public QMenu {
Q_OBJECT

public:
    explicit ClickableMenu(const QString& title, QWidget *parent = nullptr);
    ~ClickableMenu() override;

protected:
	void mouseReleaseEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    QGraphicsOpacityEffect *effect{};
    QPropertyAnimation *animation{};
};

#endif // OMPLAYER_CLICKABLEMENU_HPP
