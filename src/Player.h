#ifndef OMPLAYER_PLAYER_H
#define OMPLAYER_PLAYER_H

#include <QtAV>
#include <QtAVWidgets>

#include <QLayout>
#include <QTimer>

#include <MediaInfoDLL.h>

#include "About.h"
#include "Button.h"
#include "EventFilter.h"
#include "Label.h"
#include "PlayList.h"
#include "Settings.h"
#include "Slider.h"
#include "Widget.h"

using namespace QtAV;
using namespace MediaInfoDLL;


/**
 * Classe da interface principal.
 **********************************************************************************************************************/


class OMPlayer : public QWidget {
Q_OBJECT
public:
    explicit OMPlayer(QWidget *parent = nullptr);
    ~OMPlayer() override;

public Q_SLOTS:
    void openMedia(const QStringList &parms = QStringList());
    void onLoad();

private Q_SLOTS:
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
    void hideControls();
    void nextRand();
    void Next();
    void Previous();
    void onPaused(bool p);
    void onStart();
    void onStop();
    void setHide();
    void setShow();
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
    void onTimeSliderEnter();
    void onTimeSliderLeave();
    void updateSlider(qint64 value);
    void updateSliderUnit();
    void enablePause();
    void clickCount();
    void detectClick();
    void onMediaStatusChanged();
    static void handleError(const QtAV::AVError &error);

protected:
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    About *about{};
    AVPlayer *mediaPlayer{};
    Button *playBtn{};
    Button *stopBtn{};
    Button *nextBtn{};
    Button *previousBtn{};
    Button *replayBtn{};
    Button *shuffleBtn{};
    EventFilter *filter{};
    MediaInfo MI{};
    PlayList *playlist{};
    Label *logo{};
    Label *current{};
    Label *end{};
    QGridLayout *layout{};
    QString Width{"192"};
    QString Height{"108"};
    QStringList listnum{};
    QTimer *click{};
    QWidget *wctl{};
    Settings *sett{};
    Slider *slider{};
    VideoOutput *video{};
    VideoPreviewWidget *preview{};
    bool playing{false};
    bool pausing{false};
    bool restart{false};
    bool randplay{false};
    bool enterpos{false};
    bool isblock{false};
    bool maximize{false};
    bool showsett{false};
    int previousitem{0};
    int actualitem{0};
    int nextitem{0};
    int count{0};
    int unit{500};
};

#endif //OMPLAYER_PLAYER_H
