#ifndef OMPLAYER_EVENTFILTER_H
#define OMPLAYER_EVENTFILTER_H

#include <QObject>

class EventFilter : public QObject{
Q_OBJECT

public:
    enum UITYPE {Defaut = 0, General = 1, Control = 2};
    explicit EventFilter(QWidget *parent, EventFilter::UITYPE i = EventFilter::Defaut);
    ~EventFilter() override;
    void setMove(bool var);
    void setFixed(bool var);
    void setSett(bool var);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

Q_SIGNALS:
    void emitMouseMove();
    void emitMousePress();
    void emitMouseRelease();
    void emitDoubleClick();
    void customContextMenuRequested(QPoint point);
    void emitOpen();
    void emitShuffle();
    void emitReplay();
    void emitReplayOne();
    void emitSettings();
    void emitEsc();
    void emitEnter();
    void emitLeave();
    void emitPlay();
    void emitNext();
    void emitPrevious();

private:
    bool contextmenu{false};
    bool first{false};
    bool fixed{false};
    bool moving{false};
    bool start{false};
    bool sett{false};
    UITYPE option{EventFilter::Defaut};
};

#endif // OMPLAYER_EVENTFILTER_H
