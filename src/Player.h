#ifndef OMPLAYER_PLAYER_H
#define OMPLAYER_PLAYER_H

#include <QtAV>
#include <QtAVWidgets>

#include <QGuiApplication>
#include <QLayout>
#include <QScreen>
#include <QTimer>

#include <MediaInfoDLL.h>

#include "About.h"
#include "Button.h"
#include "EventFilter.h"
#include "Label.h"
#include "PlayList.h"
#include "Settings.h"
#include "StatisticsView.h"
#include "Slider.h"

using namespace QtAV;
using namespace MediaInfoDLL;

class OMPlayer : public QWidget {
Q_OBJECT
public:
    explicit OMPlayer(QWidget *parent = nullptr);
    ~OMPlayer() override;

public Q_SLOTS:
    void openMedia(const QStringList &parms = QStringList());
    void onLoad();

protected:
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void play(const QString &isplay, int index = (-1));
    void nextRand();
    void enterFullScreen();
    void updateChannelMenu();

private Q_SLOTS:
    void setSelect(int item);
    void ajustActualItem(int item);
    void firstPlay(const QString &name, int pos);
    void doubleplay(const QString &name);
    void setRenderer(const QString &op);
    void playPause();
    void setStop();
    void Next();
    void Previous();
    void onPaused(bool p);
    void onStart();
    void onStop();
    void setRepeat();
    void setReplay();
    void setShuffle();
    void setHide();
    void setShow();
    void hideTrue();
    void hideFalse();
    void setSettings();
    void setAbout();
    void showInfo();
    void closeDialog();
    void changeFullScreen();
    void leaveFullScreen();
    void onTimeSliderHover(int pos, int value);
    void onTimeSliderEnter();
    void onTimeSliderLeave();
    void updateSlider(qint64 value);
    void updateSliderUnit();
    void enablePause();
    void clickCount();
    void detectClick();
    void seekBySlider();
    void seekBySlider(int value);
    void seekFinished(qint64 pos);
    void setMute();
    void setVolume();
    void setVolume(int value);
    void volumeFinished(qreal pos);
    void onTimeVolume(int pos, int value);
    void onMediaStatusChanged();
    void changeChannel(QAction *action);
    void handleError(const QtAV::AVError &error);
    void enterList();
    void leaveList();
    void ShowContextMenu(const QPoint &pos);

private:
    About *about{};
    AVPlayer *mediaPlayer{};
    Button *playBtn{}, *stopBtn{};
    Button *nextBtn{}, *previousBtn{};
    Button *replayBtn{}, *shuffleBtn{};
    Button *volumeBtn{};
    EventFilter *filter{};
    Label *logo{}, *current{}, *end{};
    MediaInfo MI{};
    PlayList *playlist{};
    QAction *channelAction{};
    QMenu *channel{};
    QGridLayout *layout{};
    QSize min{906, 510}, size{};
    QSize screen{QGuiApplication::screens().at(0)->geometry().size()};
    QStringList listnum{};
    QTimer *click{};
    QWidget *wctl{};
    Settings *sett{};
    StatisticsView *infoview{};
    Slider *slider{}, *volume{};
    VideoOutput *video{};
    VideoPreviewWidget *preview{};
    bool control{false}, listmenu{false};
    bool playing{false}, pausing{false}, muted{false};
    bool restart{false}, randplay{false};
    bool enterpos{false}, showsett{false};
    bool isblock{false};
    bool invalid{false};
    bool maximize{false};
    int Width{192}, Height{108};
    int previousitem{0}, actualitem{0}, nextitem{0};
    int count{0}, unit{500};
};

#endif //OMPLAYER_PLAYER_H
