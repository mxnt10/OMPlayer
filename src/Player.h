#ifndef OMPLAYER_PLAYER_H
#define OMPLAYER_PLAYER_H

#include <QtAV>
#include <QtAVWidgets>
#include <QDoubleSpinBox>
#include <QGraphicsOpacityEffect>
#include <QGuiApplication>
#include <QLayout>
#include <QScreen>
#include <QTimer>

#include <MediaInfoDLL.h>
#include <ScreenSaver>
#include <Button>
#include <Label>
#include <Slider>

#include "About.h"
#include "EventFilter.h"
#include "PlayList.h"
#include "Settings.h"
#include "StatisticsView.h"

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
    enum STATUS { Default = 0, IsPlay = 1 };
    enum DIALOG { AboutD = 0, SettingsD = 1, InfoD = 2 };
    enum FADE { Show = 1, Hide = 2 };
    void play(const QString &isplay, int index = (-1));
    void nextRand();
    void enterFullScreen();
    void updateChannelMenu();
    void fadeWctl(OMPlayer::FADE Option);

private Q_SLOTS:
    void setTotalItems();
    void setSelect(int item);
    void ajustActualItem(int item);
    void firstPlay(const QString &name, int pos);
    void doubleplay(const QString &name);
    void setRenderer(const QString &op);
    void onSpinBoxChanged(double val);
    void switchAspectRatio(QAction *action);
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
    void setDialog(OMPlayer::DIALOG);
    void closeDialog();
    void changeFullScreen();
    void leaveFullScreen();
    void onTimeSliderHover(int pos, int value);
    void onTimeSliderEnter();
    void onTimeSliderLeave(OMPlayer::STATUS status = Default);
    void updateSlider(qint64 value);
    void updateSliderUnit();
    void enablePause();
    void clickCount();
    void detectClick();
    void seekBySlider(int value);
    void seekFinished(qint64 pos);
    void setMute();
    void setVolume(int value);
    void volumeFinished(qreal pos);
    void onTimeVolume(int pos, int value);
    void changeChannel(QAction *action);
    void enterList();
    void leaveList();
    void changeIcons(OMPlayer::STATUS change = Default);
    void onMediaStatusChanged();
    void handleError(const QtAV::AVError &error);
    void ShowContextMenu(const QPoint &pos);

private:
    About *about{};
    QtAV::AVPlayer *mediaPlayer{};
    Button *playBtn{}, *stopBtn{};
    Button *nextBtn{}, *previousBtn{};
    Button *replayBtn{}, *shuffleBtn{};
    Button *volumeBtn{};
    EventFilter *filter{};
    Label *logo{}, *current{}, *end{};
    MediaInfoDLL::MediaInfo MI{};
    PlayList *playlist{};
    QAction *channelAction{}, *aspectAction{};
    QDoubleSpinBox *speedBox{};
    QGraphicsOpacityEffect *effect{}, *effect2{};
    QLabel *ltime{}, *lvol{};
    QMenu *channel{}, *aspectratio{}, *speed{};
    QHBoxLayout *layout{};
    QSize min{906, 510}, size{};
    QSize screen{QGuiApplication::screens().at(0)->geometry().size()};
    QStringList listnum{};
    QTimer *click{};
    QWidget *wctl{}, *prev{}, *pv{}, *wvol{};
    ScreenSaver *screensaver;
    Settings *sett{};
    StatisticsView *infoview{};
    Slider *slider{}, *volume{};
    QtAV::VideoRenderer *video{};
    QtAV::VideoPreviewWidget *preview{};
    bool control{false}, listmenu{false};
    bool playing{false}, pausing{false}, muted{false};
    bool restart{false}, randplay{false};
    bool enterpos{false}, showsett{false};
    bool invalid{false}, ispreview{false};
    bool maximize{false}, fade{false}, contmenu{false};
    int Width{192}, Height{108};
    int previousitem{0}, actualitem{0}, nextitem{0};
    int count{0}, unit{500}, totalitems{0};
};

#endif //OMPLAYER_PLAYER_H
