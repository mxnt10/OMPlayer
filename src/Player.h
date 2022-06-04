#ifndef OMPLAYER_PLAYER_H
#define OMPLAYER_PLAYER_H

#include <QtAV>
#include <QtAVWidgets>

#include <QLayout>
#include <QTimer>

#include <MediaInfoDLL.h>

#include "About.h"
#include "Button.h"
#include "Label.h"
#include "PlayList.h"
#include "Settings.h"
#include "Slider.h"
#include "Widget.h"

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
    void onLoad();

private Q_SLOTS:
    void runLoad();
    void setSelect(int item);
    void ajustActualItem(int item);
    void firstPlay(const QString &name, int pos);
    void doubleplay(const QString &name);
    void setReplay();
    void setShuffle();
    void seekBySlider();
    void seekBySlider(int value);
    void ShowContextMenu(const QPoint &pos);
    void play(const QString &isplay, int index = (-1));
    void setRenderer(const QString &op);
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
    void closeAbout();
    void changeFullScreen();
    void enterFullScreen();
    void leaveFullScreen();
    void onTimeSliderHover(int pos, int value);
    void onTimeSliderEnter() const;
    void onTimeSliderLeave();
    void updateSlider(qint64 value);
    void updateSliderUnit();
    void detectClick();
    void onMediaStatusChanged();
    static void handleError(const QtAV::AVError &error);

protected:
    bool event(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    About *about;
    AVPlayer *mediaPlayer;
    Button *playBtn, *stopBtn, *nextBtn, *previousBtn;
    Button *replayBtn, *shuffleBtn;
    MediaInfo MI;
    PlayList *playlist;
    Label *logo, *current, *end;
    QGridLayout *layout;
    QString Width, Height;
    QStringList listnum;
    QTimer *click;
    QWidget *wctl;
    Settings *sett;
    Slider *slider;
    VideoOutput *video{};
    VideoPreviewWidget *preview;
    bool playing, pausing, restart, randplay;
    bool contextmenu, enterpos, isblock, maximize, moving, showsett;
    int previousitem, actualitem, nextitem, mUnit, count;
    const char a{};
};

#endif //OMPLAYER_PLAYER_H
