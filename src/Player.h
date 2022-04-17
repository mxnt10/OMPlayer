#ifndef OMPLAYER_PLAYER_H
#define OMPLAYER_PLAYER_H

#include <QtAV>
#include <QtAVWidgets>

using namespace QtAV;


/**
 * Declaração das classes.
 **********************************************************************************************************************/


namespace QtAV {
    class AVPlayer;
    class VideoOutput;
}

QT_BEGIN_NAMESPACE
class QLabel;
class QTimer;
class QString;
class QWidget;
QT_END_NAMESPACE

class Button;
class PlayList;
class Slider;


/**
 * Classe da interface principal.
 **********************************************************************************************************************/


class VideoPlayer : public QWidget {
Q_OBJECT
public:
    explicit VideoPlayer(QWidget *parent = nullptr);

    ~VideoPlayer() override;

public Q_SLOTS:

    void openMedia(const QStringList &parms = QStringList());

    static void blockScreenSaver();

    void setSelect(int item);

    void firstPlay(const QString &name);

    void play(const QString &name);

    void Next();

    void Previous();

    void seekBySlider(int value);

    void seekBySlider();

    void ShowContextMenu(const QPoint &pos);

private Q_SLOTS:

    void playPause();

    void setStop();

    void onPaused(bool p);

    void onStart();

    void onStop();

    void setHide();

    void hideTrue();

    void hideFalse();

    void setAbout();

    void changeFullScreen();

    void enterFullScreen();

    void leaveFullScreen();

    void updateSlider(qint64 value);

    void updateSliderUnit();

protected:
    bool event(QEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void enterEvent(QEvent *event) override;

    void leaveEvent(QEvent *event) override;

    void closeEvent(QCloseEvent *event) override;

private:
    AVPlayer *mediaPlayer;
    Button *playBtn, *stopBtn, *nextBtn, *previousBtn;
    PlayList *playlist;
    QLabel *logo;
    QTimer *noscreensaver;
    QWidget *about, *wctl;
    Slider *slider;
    VideoOutput *video;
    int mUnit;
    int previousitem, actualitem, nextitem;
    bool restart;
    bool contextmenu, enterpos, maximize, moving, playing;
    const char *theme;
};

#endif //OMPLAYER_PLAYER_H
