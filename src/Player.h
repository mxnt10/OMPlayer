#ifndef OMPLAYER_PLAYER_H
#define OMPLAYER_PLAYER_H

#include <QtAV>
#include <QtAVWidgets>
#include <QActionGroup>
#include <QDoubleSpinBox>
#include <QGraphicsOpacityEffect>
#include <QGuiApplication>
#include <QLayout>
#include <QScreen>
#include <QStackedWidget>
#include <QTimer>
#include <Button>
#include <CWidget>
#include <Label>
#include <ScreenSaver>
#include <Slider>

#include "About.hpp"
#include "EventFilter.hpp"
#include "PlayList.h"
#include "Settings.hpp"
#include "StatisticsView.hpp"

class OMPlayer : public QWidget {
Q_OBJECT

public:
    explicit OMPlayer(QWidget *parent = nullptr);
    ~OMPlayer() override;

public Q_SLOTS:
    void openMedia(const QStringList &parms = QStringList());

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
    void initAudioTrackMenu();
    void fadeWctl(OMPlayer::FADE Option);
    void runSubtitle();
    QString mapFirstSubtitle();

private Q_SLOTS:
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
    void changePlaylist(const QString &format, int duration);
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
    void clickCount();
    void detectClick();
    void seekBySlider(int value);
    void seekFinished(qint64 pos);
    void setMute();
    void setVolume(int value);
    void volumeFinished(qreal pos);
    void onTimeVolume(int pos, int value);
    void changeChannel(QAction *action);
    void changeAudioTrack(QAction *action);
    void changeIcons(OMPlayer::STATUS change = Default);
    void enableSubtitle(bool val);
    void charsetSubtitle();
    void onMediaStatusChanged();
    void handleError(const QtAV::AVError &error);
    void ShowContextMenu(const QPoint &pos);

    void enablePause() { if (!enterpos) pausing = true; };
    void enterList() { listmenu = true; hideFalse(); };
    void leaveList() { listmenu = false; };

private:
    About *about{};
    Button *playBtn{}, *stopBtn{}, *nextBtn{}, *previousBtn{};
    Button *replayBtn{}, *shuffleBtn{}, *volumeBtn{};
    CWidget *wctl{}, *wvol{}, *prev{};
    EventFilter *filter{};
    Label *current{}, *end{}, *lvol{}, *ltime{};
    PlayList *playlist{};
    QAction *channelAction{}, *aspectAction{}, *trackAction{};
    QActionGroup *ta{};
    QDoubleSpinBox *speedBox{};
    QGraphicsOpacityEffect *effect{}, *effect2{};
    QMenu *channel{}, *aspectratio{}, *speed{}, *audiotrack{};
    QSize min{906, 510}, size{};
    QSize screen{QGuiApplication::screens().at(0)->geometry().size()};
    QStackedWidget *stack{};
    QStringList listnum{};
    QTimer *click{};
    QWidget *pv{};
    ScreenSaver *screensaver{};
    Settings *sett{};
    StatisticsView *infoview{};
    Slider *slider{}, *volume{};
    QtAV::AVPlayer *mediaPlayer{};
    QtAV::SubtitleFilter *subtitle{};
    QtAV::VideoRenderer *video{};
    QtAV::VideoPreviewWidget *preview{};
    bool control{false}, listmenu{false}, nomousehide{false};
    bool playing{false}, pausing{false}, muted{false};
    bool restart{false}, randplay{false};
    bool enterpos{false}, showsett{false};
    bool invalid{false}, ispreview{false};
    bool maximize{false}, fade{false}, contmenu{false};
    bool prevent{false}, powersaving{false};
    int Width{192}, Height{108}, count{0}, unit{500};
    int previousitem{0}, actualitem{0}, nextitem{0};
};

#endif //OMPLAYER_PLAYER_H
