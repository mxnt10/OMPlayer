#ifndef OMPLAYER_PLAYER_H
#define OMPLAYER_PLAYER_H

#include <QtAV>
#include <QtAVWidgets>
#include <MediaInfoDLL.h>

using namespace QtAV;
using MediaInfoDLL::MediaInfo;


/**
 * Declaração das classes.
 **********************************************************************************************************************/


namespace QtAV {
    class AVPlayer;
    class VideoOutput;
    class VideoPreviewWidget;
}

namespace MediaInfoDLL {
    class MediaInfo;
}

QT_BEGIN_NAMESPACE
class QTimer;
class QWidget;
QT_END_NAMESPACE

class Button;
class Label;
class PlayList;
class Settings;
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
    void runLoad();
    void onLoad();
    void setSelect(int item);
    void ajustActualItem(int item);
    void firstPlay(const QString &name);
    void doubleplay(const QString &name);
    void setReplay();
    void setShuffle();
    void seekBySlider();
    void seekBySlider(int value);
    void ShowContextMenu(const QPoint &pos);

private Q_SLOTS:
    void play(const QString &isplay, int index = (-1));
    void playPause();
    void setStop();
    void nextRand();
    void Next();
    void Previous();
    void onPaused(bool p);
    void onStart();
    void onStop();
    void setHide();
    void hideTrue();
    void hideFalse();
    void setSettings();
    void closeSettings();
    void setAbout();
    void changeFullScreen();
    void enterFullScreen();
    void leaveFullScreen();
    void onTimeSliderHover(int pos, int value);
    void onTimeSliderEnter() const;
    void onTimeSliderLeave();
    void updateSlider(qint64 value);
    void updateSliderUnit();
    void detectClick();

protected:
    bool event(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    AVPlayer *mediaPlayer;
    Button *playBtn, *stopBtn, *nextBtn, *previousBtn;
    Button *replayBtn, *shuffleBtn;
    MediaInfo MI;
    PlayList *playlist;
    Label *logo, *current, *end;
    QString Width, Height;
    QStringList listnum;
    QTimer *click;
    QWidget *about, *wctl;
    Settings *sett;
    Slider *slider;
    VideoOutput *video;
    VideoPreviewWidget *preview;
    int mUnit, count;
    int previousitem, actualitem, nextitem;
    bool playing, pausing, restart, randplay;
    bool contextmenu, enterpos, isblock, maximize, moving, showsett;
    const char *theme;
};

#endif //OMPLAYER_PLAYER_H
