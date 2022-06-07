#ifndef OMPLAYER_EVENTFILTER_H
#define OMPLAYER_EVENTFILTER_H

#include <QObject>


/**
 * Classe de eventos principal.
 **********************************************************************************************************************/


class EventFilter : public QObject{
Q_OBJECT
public:
    explicit EventFilter(QWidget *parent);
    ~EventFilter() override;
    void setMove(bool var);
    void setFixed(bool var);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

signals:
    void emitMouseMove();
    void emitMousePress();
    void emitMouseRelease();
    void emitDoubleClick();
    void customContextMenuRequested(QPoint point);
    void emitOpen();
    void emitShuffle();
    void emitReplay();
    void emitSettings();
    void emitEsc();
    void emitLeave();


private:
    bool moving{false};
    bool fixed{false};
};


/**
 * Classe de eventos auxiliar para os controles.
 **********************************************************************************************************************/


class cEventFilter : public QObject{
Q_OBJECT
public:
    explicit cEventFilter(QWidget *parent);
    ~cEventFilter() override;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

signals:
    void emitLeave();
};

#endif // OMPLAYER_EVENTFILTER_H
