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

protected:
    void timerEvent(QTimerEvent *) override;

private:
    bool retrieveState();
    bool restoreState() const;
    bool state_saved, modified;

#ifdef Q_OS_LINUX
    bool isX11;
    int timeout;
    int interval;
    int preferBlanking;
    int allowExposures;
#endif //Q_OS_LINUX

    int ssTimerId;
};

#endif // SCREENSAVER_H
