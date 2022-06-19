#include <QLibrary>
#include <QLoggingCategory>
#include <QTimerEvent>

#include "ScreenSaver.h"

#define GRE "\x1b[38;2;000;255;000m" //Debug
#define RED "\x1b[38;2;255;050;050m" //Debug
#define CYA "\x1b[38;2;000;150;100m" //ScreenSaver

#ifdef Q_OS_LINUX

    #ifndef Success
        #define Success 0
    #endif

    struct XDisplay;
    typedef struct XDisplay Display;
    typedef Display* (*fXOpenDisplay)(const char*);
    typedef int (*fXCloseDisplay)(Display*);
    typedef int (*fXSetScreenSaver)(Display*, int, int, int, int);
    typedef int (*fXGetScreenSaver)(Display*, int*, int*, int*, int*);
    typedef int (*fXResetScreenSaver)(Display*);
    static fXOpenDisplay XOpenDisplay = nullptr;
    static fXCloseDisplay XCloseDisplay = nullptr;
    static fXSetScreenSaver XSetScreenSaver = nullptr;
    static fXGetScreenSaver XGetScreenSaver = nullptr;
    static fXResetScreenSaver XResetScreenSaver = nullptr;
    static QLibrary xlib; // NOLINT(cert-err58-cpp)

#endif //Q_OS_LINUX

#ifdef Q_OS_WIN
    #include <QAbstractEventDispatcher>
    #include <QAbstractNativeEventFilter>

    //mingw gcc4.4 EXECUTION_STATE
    #ifdef __MINGW32__
        #ifndef _WIN32_WINDOWS
            #define _WIN32_WINDOWS 0x0410
        #endif //_WIN32_WINDOWS
    #endif //__MINGW32__

    #include <windows.h>
    #define USE_NATIVE_EVENT 0

#if USE_NATIVE_EVENT
class ScreenSaverEventFilter : public QAbstractNativeEventFilter {
public:
    //screensaver is global
    static ScreenSaverEventFilter& instance() {
        static ScreenSaverEventFilter sSSEF;
        return sSSEF;
    }
    void enable(bool yes = true) {
        if (!yes) {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            mLastEventFilter = QAbstractEventDispatcher::instance()->setEventFilter(eventFilter);
#else
            QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
#endif
        } else {
            if (!QAbstractEventDispatcher::instance())
                return;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            mLastEventFilter = QAbstractEventDispatcher::instance()->setEventFilter(mLastEventFilter);
#else
            QAbstractEventDispatcher::instance()->removeNativeEventFilter(this);
#endif
        }
    }
    void disable(bool yes = true) {
        enable(!yes);
    }

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) {
        Q_UNUSED(eventType);
        MSG* msg = static_cast<MSG*>(message);
        //qDebug("ScreenSaverEventFilter: %p", msg->message);
        if (WM_DEVICECHANGE == msg->message) {
            qDebug("~~~~~~~~~~device event");
            /*if (msg->wParam == DBT_DEVICEREMOVECOMPLETE) {
                qDebug("Remove device");
            }*/

        }
        if (msg->message == WM_SYSCOMMAND
                && ((msg->wParam & 0xFFF0) == SC_SCREENSAVE
                    || (msg->wParam & 0xFFF0) == SC_MONITORPOWER)
        ) {
            //qDebug("WM_SYSCOMMAND SC_SCREENSAVE SC_MONITORPOWER");
            if (result) {
                //*result = 0; //why crash?
            }
            return true;
        }
        return false;
    }
private:
    ScreenSaverEventFilter() {}
    ~ScreenSaverEventFilter() {}
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    static QAbstractEventDispatcher::EventFilter mLastEventFilter;
    static bool eventFilter(void* message) {
        return ScreenSaverEventFilter::instance().nativeEventFilter("windows_MSG", message, 0);
    }
#endif
};
        #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            QAbstractEventDispatcher::EventFilter ScreenSaverEventFilter::mLastEventFilter = 0;
        #endif
    #endif //USE_NATIVE_EVENT
#endif //Q_OS_WIN


ScreenSaver& ScreenSaver::instance() {
    static ScreenSaver sSS;
    return sSS;
}

ScreenSaver::ScreenSaver() {
    #ifdef Q_OS_LINUX
        if (!qEnvironmentVariableIsEmpty("DISPLAY")) {
            xlib.setFileName(QString::fromLatin1("libX11.so"));
            isX11 = xlib.load();
            if (!isX11) {
                xlib.setFileName(QString::fromLatin1("libX11.so.6"));
                isX11 = xlib.load();
            }
            if (!isX11) {
                qDebug("open X11 so failed: %s", xlib.errorString().toUtf8().constData());
            } else {
                XOpenDisplay = (fXOpenDisplay)xlib.resolve("XOpenDisplay");
                XCloseDisplay = (fXCloseDisplay)xlib.resolve("XCloseDisplay");
                XSetScreenSaver = (fXSetScreenSaver)xlib.resolve("XSetScreenSaver");
                XGetScreenSaver = (fXGetScreenSaver)xlib.resolve("XGetScreenSaver");
                XResetScreenSaver = (fXResetScreenSaver)xlib.resolve("XResetScreenSaver");
            }
            isX11 = XOpenDisplay && XCloseDisplay && XSetScreenSaver && XGetScreenSaver && XResetScreenSaver;
        }
    #endif //Q_OS_LINUX
    retrieveState();
}

ScreenSaver::~ScreenSaver() {
    restoreState();
    #ifdef Q_OS_LINUX
        if (xlib.isLoaded()) xlib.unload();
    #endif
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
#pragma ide diagnostic ignored "UnusedValue"
#pragma ide diagnostic ignored "ConstantConditionsOC"

bool ScreenSaver::enable(bool yes) {
    bool rv = false;

    #if defined(Q_OS_WIN) && !defined(Q_OS_WINRT)
        #if USE_NATIVE_EVENT
            ScreenSaverEventFilter::instance().enable(yes);
            modified = true;
            rv = true;
            return true;
        #else
            static EXECUTION_STATE sLastState = 0;
            if (!yes) {
                sLastState = SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
            } else {
                if (sLastState)
                    sLastState = SetThreadExecutionState(sLastState|ES_CONTINUOUS);
            }
            rv = sLastState != 0;
            modified = true;
        #endif //USE_NATIVE_EVENT
    #endif //defined(Q_OS_WIN) && !defined(Q_OS_WINRT)

    #ifdef Q_OS_LINUX
        if (isX11) {
            Display *display = XOpenDisplay(nullptr);
            int ret = 0;
            if (yes)
                ret = XSetScreenSaver(display, -1, interval, preferBlanking, allowExposures);
            else
                ret = XSetScreenSaver(display, 0, interval, preferBlanking, allowExposures);
            ret = XResetScreenSaver(display);
            XCloseDisplay(display);
            rv = ret==Success;
            qDebug("%s(%sScreenSaver%s)%s::%sEnable %d, ret %d timeout origin: %d ...\033[0m",
                   GRE, RED, GRE, RED, CYA, yes, ret, timeout);
        }
        modified = true;
        if (!yes) {
            if (ssTimerId <= 0) {
                ssTimerId = startTimer(1000 * 60);
            }
        } else {
            if (ssTimerId)
                killTimer(ssTimerId);
        }
        rv = modified = true;
    #endif //Q_OS_LINUX

    if (!rv) {
        qWarning("Failed to enable screen saver (%d)", yes);
    } else {
        if (yes)
            qDebug("%s(%sScreenSaver%s)%s::%sSucceed to enable screen saver (%d) ...\033[0m", GRE, RED, GRE, RED, CYA, yes);
        else
            qDebug("%s(%sScreenSaver%s)%s::%sSucceed to disable screen saver (%d) ...\033[0m", GRE, RED, GRE, RED, CYA, yes);
    }
    return rv;
}

#pragma clang diagnostic pop

void ScreenSaver::enable() {
    enable(true);
}

void ScreenSaver::disable() {
    enable(false);
}

#ifdef Q_OS_LINUX
void ScreenSaver::active() {
    qDebug("%s(%sScreenSaver%s)%s::%sEnable block screen saver ...\033[0m", GRE, RED, GRE, RED, CYA);
    activeBlock = true;
}

void ScreenSaver::deactive() {
    qDebug("%s(%sScreenSaver%s)%s::%sDisable block screen saver ...\033[0m", GRE, RED, GRE, RED, CYA);
    activeBlock = false;
}
#endif //Q_OS_LINUX

bool ScreenSaver::retrieveState() {
    bool rv = false;
    if (!state_saved) {
        #ifdef Q_OS_LINUX
            if (isX11) {
                Display *display = XOpenDisplay(nullptr);
                XGetScreenSaver(display, &timeout, &interval, &preferBlanking, &allowExposures);
                XCloseDisplay(display);
                qDebug("%s(%sScreenSaver%s)%s::%sRetrieveState timeout: %d, interval: %d, preferBlanking: %d, allowExposures: %d ...\033[0m",
                       GRE, RED, GRE, RED, CYA, timeout, interval, preferBlanking, allowExposures);
                state_saved = rv = true;
            }
        #endif //Q_OS_LINUX
    } else {
        qDebug("ScreenSaver::retrieveState: state already saved previously, doing nothing");
    }
    return rv;
}

bool ScreenSaver::restoreState() const {
    bool rv = false;
    if (!modified) {
        qDebug("ScreenSaver::restoreState: state did not change, doing nothing");
        return true;
    }
    if (state_saved) {

    #if defined(Q_OS_WIN) && !defined(Q_OS_WINRT)
        #if USE_NATIVE_EVENT
            ScreenSaverEventFilter::instance().enable();
            rv = true;
        #else
            SetThreadExecutionState(ES_CONTINUOUS);
        #endif //USE_NATIVE_EVENT
    #endif //defined(Q_OS_WIN) && !defined(Q_OS_WINRT)

    #ifdef Q_OS_LINUX
        if (isX11) {
            Display *display = XOpenDisplay(nullptr);
            XSetScreenSaver(display, timeout, interval, preferBlanking, allowExposures);
            XCloseDisplay(display);
            rv = true;
        }
    #endif //Q_OS_LINUX

    } else {
        qWarning("ScreenSaver::restoreState: no data, doing nothing");
    }
    return rv;
}

void ScreenSaver::timerEvent(QTimerEvent *e) {
    if (e->timerId() != ssTimerId) return;
    #ifdef Q_OS_LINUX
        if (!isX11 || !activeBlock) return;
        qDebug("%s(%sScreenSaver%s)%s::%sActivating anti-lock ...\033[0m", GRE, RED, GRE, RED, CYA);
        Display *display = XOpenDisplay(nullptr);
        XResetScreenSaver(display);
        XCloseDisplay(display);
    #endif //Q_OS_LINUX
}
