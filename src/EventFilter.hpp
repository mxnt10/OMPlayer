#ifndef OMPLAYER_EVENTFILTER_HPP
#define OMPLAYER_EVENTFILTER_HPP

#include <QObject>

class EventFilter : public QObject{
Q_OBJECT

public:
    enum Type {NormalEvent = 0, ControlEvent = 1};
    explicit EventFilter(QWidget *parent, EventFilter::Type i = EventFilter::NormalEvent);
    ~EventFilter() override;
    void setMove(bool var);
    void setFixed(bool var);
    void setSett(bool var);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;


#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"

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

#pragma clang diagnostic pop


private:
    bool contextmenu{false}, fixed{false}, moving{false}, sett{false};
    Type option{EventFilter::NormalEvent};
};

#endif //OMPLAYER_EVENTFILTER_HPP
