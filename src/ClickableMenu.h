#ifndef CLICKABLEMENU_H
#define CLICKABLEMENU_H

#include <QMenu>

class ClickableMenu : public QMenu {
Q_OBJECT
public:
    explicit ClickableMenu(const QString& title, QWidget *parent = nullptr);
    ~ClickableMenu() override;

protected:
	void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // CLICKABLEMENU_H
