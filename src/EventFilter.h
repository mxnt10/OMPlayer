#ifndef OMPLAYER_EVENTFILTER_H
#define OMPLAYER_EVENTFILTER_H

#include <QObject>


/**
 * Classe de eventos principal.
 **********************************************************************************************************************/


class EventFilter : public QObject{
Q_OBJECT
public:
    explicit EventFilter(QWidget *parent, int i);
    ~EventFilter() override;
    void setMove(bool var);
    void setFixed(bool var);
    void setSett(bool var);

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

private slots:
    void startShow();

private:
    bool contextmenu{false};
    bool first{false};
    bool fixed{false};
    bool moving{false};
    bool start{false};
    bool sett{false};
    int num{0};
};

#endif // OMPLAYER_EVENTFILTER_H
