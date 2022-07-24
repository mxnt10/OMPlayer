#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <QObject>

class ScreenSaver : QObject {
Q_OBJECT
public:
    static ScreenSaver& instance();
    ScreenSaver();
    ~ScreenSaver() override;
    bool enable(bool yes);

public slots:
    void enable();
    void disable();

#ifdef Q_OS_LINUX
    void active();
    void deactive();
#endif //Q_OS_LINUX

protected:
    void timerEvent(QTimerEvent *) override;

private:
    bool retrieveState();
    bool restoreState() const;
    bool state_saved{false};
    bool modified{false};
    int ssTimerId{0};

#ifdef Q_OS_LINUX
    bool activeBlock{false};
    bool isX11{false};
    int timeout{0};
    int interval{0};
    int preferBlanking{0};
    int allowExposures{0};
#endif //Q_OS_LINUX
};

#endif // SCREENSAVER_H
