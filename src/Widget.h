#ifndef OMPLAYER_WIDGET_H
#define OMPLAYER_WIDGET_H

#include <QWidget>

/**********************************************************************************************************************/

class Widget : public QWidget {
Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

signals:
    void emitEnter();
    void emitLeave();
};

#endif //OMPLAYER_WIDGET_H
