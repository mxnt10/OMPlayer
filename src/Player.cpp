#include <QMenu>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QWidgetAction>
#include <CustomMenu>
#include <Frame>
#include <Utils>

#include "Extensions.hpp"
#include "VUMeterFilter.hpp"
#include "Player.h"

#define ClickableMenu(x) ClickableMenu(x + "\t\t")


/**********************************************************************************************************************/


/** Construtor que define a interface do programa */
OMPlayer::OMPlayer(QWidget *parent) : QWidget(parent) {
    qDebug("%s(%sInterface%s)%s::%sIniciando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, RED, CYA);
    this->setWindowTitle(QString(PRG_NAME));
    this->setWindowIcon(QIcon(Utils::setIcon()));
    this->setMinimumSize(min);
    this->setMouseTracking(true);
    this->setAutoFillBackground(true);
    this->setStyleSheet(Utils::setStyle("global"));
    this->move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center());
    double vol = JsonTools::floatJson("volume");
    int max_vol = JsonTools::intJson("max_volume");


    /** Fundo preto */
    QPalette pal{};
    pal.setColor(QPalette::Window, Qt::black);
    this->setPalette(pal);


    /** Preview do reprodutor */
    prev = new CWidget((Qt::Tool | Qt::FramelessWindowHint));
    pv = new QWidget(prev);
    pv->setStyleSheet(Utils::setStyle("widget"));
    preview = new QtAV::VideoPreviewWidget(pv);
    preview->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    ltime = new Label(pv);
    wvol = new CWidget((Qt::Tool | Qt::FramelessWindowHint));
    lvol = new Label(CENTER, 42, 22, wvol);
    lvol->setStyleSheet(Utils::setStyle("label"));


    /** Janelas de configurações do programa, informações e demais */
    about = new About(this);
    sett = new Settings(this);
    infoview = new StatisticsView(this);
    screensaver = new ScreenSaver(this);
    connect(sett, &Settings::emitcolor, [this](const QColor &c){ if (playing) subtitle->setColor(c); });
    connect(sett, &Settings::emitfont, [this](const QFont &f){ if (playing) subtitle->setFont(f); });
    connect(sett, &Settings::emitEngine, [this](const QString &s){ if (playing) subtitle->setEngines({s});});
    connect(sett, &Settings::emitEnableSub, this, &OMPlayer::enableSubtitle);
    connect(sett, &Settings::emitCharset, this, &OMPlayer::charsetSubtitle);
    connect(sett, &Settings::changethemeicon, [this](){ changeIcons(); });
    connect(sett, &Settings::emitvalue, this, &OMPlayer::setRenderer);
    connect(sett, &Settings::onclose, this, &OMPlayer::closeDialog);
    connect(infoview, &StatisticsView::onclose, this, &OMPlayer::closeDialog);
    connect(infoview, &StatisticsView::emitFormat, this, &OMPlayer::changePlaylist);
    connect(about, &About::onclose, this, &OMPlayer::closeDialog);


    /** Parte principal do programa que permite o funcionamento do reprodutor */
    mediaPlayer = new QtAV::AVPlayer(this);
    connect(mediaPlayer->audio(), &QtAV::AudioOutput::volumeChanged, this, &OMPlayer::volumeFinished);
    connect(mediaPlayer, &QtAV::AVPlayer::mediaStatusChanged, this, &OMPlayer::onMediaStatusChanged);
    connect(mediaPlayer, &QtAV::AVPlayer::seekFinished, this, &OMPlayer::seekFinished);
    connect(mediaPlayer, &QtAV::AVPlayer::positionChanged, this, &OMPlayer::updateSlider);
    connect(mediaPlayer, &QtAV::AVPlayer::notifyIntervalChanged, this, &OMPlayer::updateSliderUnit);
    connect(mediaPlayer, &QtAV::AVPlayer::started, this, &OMPlayer::onStart);
    connect(mediaPlayer, &QtAV::AVPlayer::paused, this, &OMPlayer::onPaused);
    connect(mediaPlayer, &QtAV::AVPlayer::error, this, &OMPlayer::handleError);


    /** Filtro de áudio */
    auto vu = new VUMeterFilter();
    mediaPlayer->installFilter(vu);
    connect(vu, &VUMeterFilter::leftLevelChanged, infoview, &StatisticsView::setLeftDB);
    connect(vu, &VUMeterFilter::rightLevelChanged, infoview, &StatisticsView::setRightDB);


    /** Playlist do reprodutor */
    playlist = new PlayList();
    connect(playlist, &PlayList::aboutToPlay, this, &OMPlayer::doubleplay);
    connect(playlist, &PlayList::firstPlay, this, &OMPlayer::firstPlay);
    connect(playlist, &PlayList::selected, this, &OMPlayer::setSelect);
    connect(playlist, &PlayList::emitremove, this, &OMPlayer::ajustActualItem);
    connect(playlist, &PlayList::emithide, this, &OMPlayer::hideTrue);
    connect(playlist, &PlayList::emitnohide, this, &OMPlayer::hideFalse);
    connect(playlist, &PlayList::enterListView, this, &OMPlayer::enterList);
    connect(playlist, &PlayList::leaveListView, this, &OMPlayer::leaveList);
    connect(playlist, &PlayList::emitstop, this, &OMPlayer::setStop);
    connect(playlist, &PlayList::nomousehide, [this](){ nomousehide = true; });


    /** Barra de progresso de execução e Labels */
    current = new Label(CENTER, 70, "-- -- : -- --");
    end = new Label(CENTER, 70, "-- -- : -- --");
    slider = new Slider(this, true, null, 28, 0, "slider", "slider-hover");
    connect(slider, &Slider::onHover, this, &OMPlayer::onTimeSliderHover);
    connect(slider, &Slider::sliderMoved, this, &OMPlayer::seekBySlider);
    connect(slider, &Slider::emitEnter, this, &OMPlayer::onTimeSliderEnter);
    connect(slider, &Slider::emitLeave, [this](){ onTimeSliderLeave(); });
    connect(slider, &Slider::sliderPressed, [this](){ seekBySlider(slider->value()); });


    /** Botões para os controles de reprodução */
    playBtn = new Button(Button::NormalBtn, 48, "play");
    stopBtn = new Button(Button::NormalBtn, 32, "stop");
    nextBtn = new Button(Button::LoopBtn, 32, "next", "forward");
    previousBtn = new Button(Button::LoopBtn, 32, "previous", "backward");
    replayBtn = new Button(Button::DoubleBtn, 32, "replay", "replay-one");
    shuffleBtn = new Button(Button::NormalBtn, 32, "shuffle");
    volumeBtn = new Button(Button::NormalBtn, 32, "volume_high");
    connect(playBtn, &Button::clicked, this, &OMPlayer::playPause);
    connect(stopBtn, &Button::clicked, this, &OMPlayer::setStop);
    connect(nextBtn, &Button::customPress, this, &OMPlayer::Next);
    connect(previousBtn, &Button::customPress, this, &OMPlayer::Previous);
    connect(nextBtn, &Button::loopPress, mediaPlayer, &QtAV::AVPlayer::seekForward);
    connect(previousBtn, &Button::loopPress, mediaPlayer, &QtAV::AVPlayer::seekBackward);
    connect(replayBtn, &Button::customPress, this, &OMPlayer::setReplay);
    connect(replayBtn, &Button::longPress, this, &OMPlayer::setRepeat);
    connect(shuffleBtn, &Button::clicked, this, &OMPlayer::setShuffle);
    connect(volumeBtn, &Button::clicked, this, &OMPlayer::setMute);


    /** Verificando se as opções de replay e aleatório estão ativas */
    if (JsonTools::boolJson("on_replayone")) setRepeat();
    if (JsonTools::boolJson("on_replay")) setReplay();
    if (JsonTools::boolJson("on_shuffle")) setShuffle();


    /** Controle do volume */
    volume = new Slider(this, false, 100, null, max_vol, "volume");
    volume->setValue(int(vol * max_vol));
    mediaPlayer->audio()->setVolume(vol);
    connect(volume, &Slider::onHover, this, &OMPlayer::onTimeVolume);
    connect(volume, &Slider::sliderMoved, this, &OMPlayer::setVolume);
    connect(volume, &Slider::sliderPressed, [this](){ setVolume(volume->value()); });
    connect(volume, &Slider::emitLeave, wvol, &QWidget::close);

    /** Plano de fundo semitransparente dos controles de reprodução */
    qDebug("%s(%sInterface%s)%s::%sPreparando o layout da interface ...\033[0m", GRE, RED, GRE, RED, CYA);
    auto *bgcontrol = new QWidget();
    bgcontrol->setMaximumHeight(120);
    bgcontrol->setStyleSheet(Utils::setStyle("widget"));
    auto *bgctl = new QVBoxLayout();
    bgctl->setMargin(10);
    bgctl->addWidget(bgcontrol);


    /** Layout dos botões */
    auto line = new Line::Frame(Line::Frame::Vertical, 25);
    auto *buttons = new QHBoxLayout();
    buttons->addStretch(1);
    buttons->addSpacing(20);
    buttons->addWidget(replayBtn);
    buttons->addWidget(shuffleBtn);
    buttons->addSpacing(5);
    buttons->addWidget(line);
    buttons->addSpacing(5);
    buttons->addWidget(stopBtn);
    buttons->addWidget(previousBtn);
    buttons->addWidget(playBtn);
    buttons->addWidget(nextBtn);
    buttons->addWidget(volumeBtn);
    buttons->addSpacing(5);
    buttons->addWidget(volume);
    buttons->addStretch(1);


    /** Ajustes na barra de execução para inserir o tempo de execução e a duração */
    auto *fgslider = new QHBoxLayout();
    fgslider->addWidget(current);
    fgslider->addWidget(slider);
    fgslider->addWidget(end);


    /** Conteiner dos controles */
    auto *fctl = new QWidget();
    auto *ffilter = new EventFilter(this, EventFilter::ControlEvent);
    fctl->installEventFilter(ffilter);
    connect(ffilter, &EventFilter::emitLeave, this, &OMPlayer::hideTrue);
    connect(ffilter, &EventFilter::emitEnter, this, &OMPlayer::hideFalse);


    /** Ajuste dos controles */
    auto *fgctl = new QVBoxLayout(fctl);
    fgctl->setContentsMargins(10, 12, 10, 22);
    fgctl->addLayout(fgslider);
    fgctl->addLayout(buttons);


    /** Layout dos controles de reprodução */
    wctl = new CWidget(this, (Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint));
    auto *controls = new QGridLayout(wctl);
    controls->setMargin(0);
    controls->setSpacing(0);
    controls->addWidget(playlist, 0, 0);
    controls->addLayout(bgctl, 1, 0);
    controls->addWidget(fctl, 1, 0);
    controls->setAlignment(BOTTON);


    /** Efeito de transparência funcional para a playlist. O setWindowOpacity() não rolou. */
    effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(1.0);
    effect2 = new QGraphicsOpacityEffect(this);
    effect2->setOpacity(1.0);
    bgcontrol->setGraphicsEffect(effect);
    fctl->setGraphicsEffect(effect2);


    /** Definição e Layout da logo */
    auto *wlogo = new TrackWidget();
    auto logo = new Label(CENTER, Utils::setIcon());
    auto wlayout = new TrackWidget();
    auto *llogo = new QGridLayout(wlayout);
    llogo->setMargin(0);
    llogo->addWidget(logo, 0, 0);
    llogo->addWidget(wlogo, 0, 0);


    /** Lista de widgets do programa */
    stack = new QStackedWidget();
    stack->setMouseTracking(true);
    stack->addWidget(wlayout);
    setRenderer(JsonTools::stringJson("renderer"));
    stack->setCurrentIndex(0);


    /** Layout principal para os widgets */
    auto layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->addWidget(stack);
    this->setLayout(layout);


    /** Filtro de eventos */
    filter = new EventFilter(this);
    auto *cfilter = new EventFilter(this, EventFilter::ControlEvent);
    this->installEventFilter(filter);
    wctl->installEventFilter(filter);
    wctl->installEventFilter(cfilter);
    connect(filter, &EventFilter::emitMouseMove, this, &OMPlayer::setShow);
    connect(filter, &EventFilter::emitDoubleClick, this, &OMPlayer::changeFullScreen);
    connect(filter, &EventFilter::emitMousePress, this, &OMPlayer::enablePause);
    connect(filter, &EventFilter::emitMouseRelease, this, &OMPlayer::clickCount);
    connect(filter, &EventFilter::customContextMenuRequested, this, &OMPlayer::ShowContextMenu);
    connect(filter, &EventFilter::emitEsc, this, &OMPlayer::leaveFullScreen);
    connect(filter, &EventFilter::emitShuffle, this, &OMPlayer::setShuffle);
    connect(filter, &EventFilter::emitReplay, this, &OMPlayer::setReplay);
    connect(filter, &EventFilter::emitReplayOne, this, &OMPlayer::setRepeat);
    connect(filter, &EventFilter::emitPlay, this, &OMPlayer::playPause);
    connect(filter, &EventFilter::emitNext, this, &OMPlayer::Next);
    connect(filter, &EventFilter::emitPrevious, this, &OMPlayer::Previous);
    connect(filter, &EventFilter::emitLeave, this, &OMPlayer::setHide);
    connect(filter, &EventFilter::emitOpen, [this](){ openMedia(); });
    connect(filter, &EventFilter::emitSettings, [this](){ setDialog(OMPlayer::SettingsD); });
    connect(cfilter, &EventFilter::emitLeave, [this](){
        if (!contmenu) playlist->hideFade();
        setHide();
    });


    /** Temporizador para o cálculo do número de cliques em 300ms */
    click = new QTimer(this);
    click->setSingleShot(true);
    click->setInterval(300);
    connect(click, &QTimer::timeout, this, &OMPlayer::detectClick);


    /** Menu dos canais de áudio para o menu de contexto */
    auto *subMenu = new ClickableMenu(tr("Channel"));
    Utils::changeMenuIcon(subMenu, "channel");
    channel = new QMenu(this);
    channel = subMenu;
    connect(subMenu, &ClickableMenu::triggered, this, &OMPlayer::changeChannel);
    subMenu->addAction(tr("As input"))->setData(QtAV::AudioFormat::ChannelLayout_Unsupported);
    subMenu->addAction(tr("Stereo"))->setData(QtAV::AudioFormat::ChannelLayout_Stereo);
    subMenu->addAction(tr("Mono (Center)"))->setData(QtAV::AudioFormat::ChannelLayout_Center);
    subMenu->addAction(tr("Left"))->setData(QtAV::AudioFormat::ChannelLayout_Left);
    subMenu->addAction(tr("Right"))->setData(QtAV::AudioFormat::ChannelLayout_Right);
    auto *ag = new QActionGroup(subMenu);
    ag->setExclusive(true);


    /** Verificando o que foi clicado */
    foreach(QAction* action, subMenu->actions()) {
        ag->addAction(action);
        action->setCheckable(true);
    }


    /** Menu dos dimensões de tela para o menu de contexto */
    subMenu = new ClickableMenu(tr("Aspect Ratio"));
    Utils::changeMenuIcon(subMenu, "aspectratio");
    aspectratio = new QMenu(this);
    aspectratio = subMenu;
    aspectAction = subMenu->addAction(Utils::aspectStr(Utils::AspectVideo));
    aspectAction->setData(Utils::aspectNum(Utils::AspectVideo));
    connect(subMenu, &ClickableMenu::triggered, this, &OMPlayer::switchAspectRatio);
    subMenu->addAction(Utils::aspectStr(Utils::AspectAuto))->setData(Utils::AspectAuto);
    subMenu->addAction(Utils::aspectStr(Utils::Aspect11  ))->setData(Utils::Aspect11);
    subMenu->addAction(Utils::aspectStr(Utils::Aspect21  ))->setData(Utils::Aspect21);
    subMenu->addAction(Utils::aspectStr(Utils::Aspect32  ))->setData(Utils::Aspect32);
    subMenu->addAction(Utils::aspectStr(Utils::Aspect43  ))->setData(Utils::Aspect43);
    subMenu->addAction(Utils::aspectStr(Utils::Aspect54  ))->setData(Utils::Aspect54);
    subMenu->addAction(Utils::aspectStr(Utils::Aspect118 ))->setData(Utils::Aspect118);
    subMenu->addAction(Utils::aspectStr(Utils::Aspect149 ))->setData(Utils::Aspect149);
    subMenu->addAction(Utils::aspectStr(Utils::Aspect1410))->setData(Utils::Aspect1410);
    subMenu->addAction(Utils::aspectStr(Utils::Aspect169 ))->setData(Utils::Aspect169);
    subMenu->addAction(Utils::aspectStr(Utils::Aspect1610))->setData(Utils::Aspect1610);
    subMenu->addAction(Utils::aspectStr(Utils::Aspect235 ))->setData(Utils::Aspect235);
    ag = new QActionGroup(subMenu);
    ag->setExclusive(true);


    /** Verificando o que foi clicado */
    foreach(QAction* action, subMenu->actions()) {
        ag->addAction(action);
        action->setCheckable(true);
    }
    aspectAction->setChecked(true);


    /** Menu de velocidade para o menu de contexto */
    subMenu = new ClickableMenu(tr("Speed"));
    Utils::changeMenuIcon(subMenu, "speed");
    speed = new QMenu(this);
    speed = subMenu;
    speedBox = new QDoubleSpinBox();
    speedBox->setRange(0.20, 10);
    speedBox->setValue(1.0);
    speedBox->setSingleStep(0.20);
    connect(speedBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double d){ onSpinBoxChanged(d); });
    auto *actspeed = new QWidgetAction(nullptr);
    actspeed->setDefaultWidget(speedBox);
    subMenu->addAction(actspeed);


    /** Menu de saídas de áudio para o menu de contexto */
    subMenu = new ClickableMenu(tr("Audio Track"));
    Utils::changeMenuIcon(subMenu, "track");
    audiotrack = new QMenu(this);
    audiotrack = subMenu;
    connect(subMenu, &ClickableMenu::triggered, this, &OMPlayer::changeAudioTrack);
    ta = new QActionGroup(subMenu);
    ta->setExclusive(true);
    initAudioTrackMenu();
}


/** Destrutor */
OMPlayer::~OMPlayer() = default;


/**********************************************************************************************************************/


/** Função para abrir arquivos multimídia */
void OMPlayer::openMedia(const QStringList &parms) {
    if(!parms.isEmpty()) playlist->addItems(parms);
    else {
        nomousehide = true;
        playlist->getFiles();
    }
}


/** Setando opções de Rendenização */
void OMPlayer::setRenderer(const QString &op) {
    if (stack->count() == 2) stack->removeWidget(video->widget());

    /** Para mudar a renderização é necessário recriar o widget removido para depois ser readicionado.
     * O uso do QStackWidget vai garantir que o widget vai ser exibido no lugar da logo sem problemas. */

    struct {
        const char* name;
        QtAV::VideoRendererId id;
    } rend[] = {{"OpenGL",     QtAV::VideoRendererId_OpenGLWidget },
                {"QGLWidget2", QtAV::VideoRendererId_GLWidget2    },
                {"Direct2D",   QtAV::VideoRendererId_Direct2D     },
                {"GDI",        QtAV::VideoRendererId_GDI          },
                {"XVideo",     QtAV::VideoRendererId_XV           },
                {"X11",        QtAV::VideoRendererId_X11          },
                {"QGLWidget",  QtAV::VideoRendererId_GLWidget     },
                {"Widget",     QtAV::VideoRendererId_Widget       }, {nullptr, 0}};

    for (int i = 0; rend[i].name; ++i) {
        if (QString::compare(op, rend[i].name, Qt::CaseSensitive) == 0) {
            video = QtAV::VideoRenderer::create(rend[i].id);
            video->widget()->setMouseTracking(true);
            break;
        }
    }

    /** Ao recriar o widget, o índice do QStackWidget precisará ser definido novamente */
    if (!video && !video->isAvailable()) video = new QtAV::VideoOutput(this);
    if (subtitle) subtitle->uninstall();
    mediaPlayer->setRenderer(video);
    stack->addWidget(video->widget());
    if (playing) stack->setCurrentIndex(1);

    const QtAV::VideoRendererId vid = mediaPlayer->renderer()->id();
    if (vid == QtAV::VideoRendererId_XV ||
        vid == QtAV::VideoRendererId_GLWidget2 ||
        vid == QtAV::VideoRendererId_OpenGLWidget)
        mediaPlayer->renderer()->forcePreferredPixelFormat(true);
    else mediaPlayer->renderer()->forcePreferredPixelFormat(false);

    if (subtitle) subtitle->installTo(video);
    if (video) qDebug("%s(%sSettings%s)%s::%sSetando rendenização %s %i ...\033[0m",
                      GRE, RED, GRE, RED, BLU, STR(op), video->id());
}


/** Alterando a dimensão de tela */
void OMPlayer::switchAspectRatio(QAction *action) {
    Utils::AspectRatio r = Utils::AspectRatio(action->data().toInt());
    if (action == aspectAction) return;

    qDebug("%s(%sMenu%s)%s::%sSetando aspect ratio %s ...\033[0m", GRE, RED, GRE, RED, BLU, STR(Utils::aspectStr(r)));
    if (r == Utils::AspectVideo) mediaPlayer->renderer()->setOutAspectRatioMode(QtAV::VideoRenderer::VideoAspectRatio);
    else if (r == Utils::AspectAuto)
        mediaPlayer->renderer()->setOutAspectRatioMode(QtAV::VideoRenderer::RendererAspectRatio);
    else {
        mediaPlayer->renderer()->setOutAspectRatioMode(QtAV::VideoRenderer::CustomAspectRation);
        mediaPlayer->renderer()->setOutAspectRatio(Utils::aspectNum(r));
    }
    aspectAction->setChecked(false);
    aspectAction = action;
    aspectAction->setChecked(true);
}


/** Controle da velocidade do reprodutor */
void OMPlayer::onSpinBoxChanged(double val) {
    if (playing && !mediaPlayer->isPaused() && mediaPlayer->isPlaying()) {
        qDebug("%s(%sMenu%s)%s::%sSetando velocidade de reprodução: %f ...\033[0m", GRE, RED, GRE, RED, BLU, val);
        mediaPlayer->setSpeed(val);
    }
    else speedBox->setValue(1);
}


/** Função que possui o propósito de ajustar a ordem de execução ao remover itens da playlist anteriores ao
 * arquivo multimídia sendo reproduzido no momento */
void OMPlayer::ajustActualItem(int item) {
    listnum.clear();  /** É preferível redefinir a contagem aleatória */
    if (!playing) return;

    if (item < actualitem) {
        qDebug("%s(%sPlayer%s)%s::%sRemovido item anterior %i ...\033[0m", GRE, RED, GRE, RED, ORA, item);
        playlist->selectCurrent(previousitem);
        actualitem = previousitem;
        nextitem = actualitem + 1;
        previousitem = actualitem - 1;
        if (actualitem == 0) previousitem = playlist->setListSize() - 1;
        if (actualitem == playlist->setListSize() - 1) nextitem = 0;
        return;
    }

    if (item == actualitem) {
        qDebug("%s(%sPlayer%s)%s::%sRemovido item atual %i ...\033[0m", GRE, RED, GRE, RED, ORA, item);
        if (actualitem == playlist->setListSize() && !restart && !randplay) {
            playing = false;
            mediaPlayer->stop();
            playlist->selectClean();
            previousitem = playlist->setListSize() - 1;
            actualitem = 0;
            nextitem = 1;
        } else play(playlist->getItems(actualitem), actualitem);
        return;
    }

    if (item > actualitem) {
        qDebug("%s(%sPlayer%s)%s::%sRemovido item posterior %i ...\033[0m", GRE, RED, GRE, RED, ORA, item);
        playlist->selectCurrent(actualitem);
    }
}


/** Setando o item atualmente selecionado */
void OMPlayer::setSelect(int item) {
    if (!playing) {
        actualitem = item;
        qDebug("%s(%sPlaylist%s)%s::%sSelecionando item %s manualmente ...\033[0m",
               GRE, RED, GRE, RED, ORA, STR(Utils::mediaTitle(playlist->getItems(item))));
    }
}


/** Função geral para execução de arquivos multimídia */
void OMPlayer::play(const QString &isplay, int index) {
    this->setWindowTitle(Utils::mediaTitle(isplay));
    speedBox->setValue(1);
    mediaPlayer->setSpeed(1);
    runSubtitle();

    /** Reproduzindo */
    mediaPlayer->stop();
    mediaPlayer->setPriority(sett->decoderPriorityNames());
    mediaPlayer->setOptionsForVideoCodec(sett->videoDecoderOptions());
    mediaPlayer->play(isplay);

    /** Atualizando as variáveis de controle */
    if (index > (-1)) {
        playlist->selectCurrent(index);

        /** Cálculo dos próximos itens a serem executados. O cálculo foi baseado na seguinte forma, uma lista começa
         * do 0. Portanto, uma lista de 10 itens vai vai de 0 a 9.
         *
         * O primeiro item selecionado da playlist vai ser 0, e o item anterior vai ser contabilizado como -1, que não
         * existe na lista. Por isso, é necessário o ajuste.
         *
         * A função setListSize() retorna o número de itens e não a posição, necessário subtrair 1.
         *
         * Já ao selecionar o último número da playlist, o próximo item vai exceder o número de posições e como a ideia
         * é que a função Next() ao chegar ao último item retorne para o ínicio, então o valor é corrigido para 0. */

        actualitem = index;
        nextitem = actualitem + 1;
        previousitem = actualitem - 1;
        if (actualitem == 0) previousitem = playlist->setListSize() - 1;
        if (actualitem == playlist->setListSize() - 1) nextitem = 0;
        prevent = false;
    }
}


/** Para executar os itens recém adicionados da playlist */
void OMPlayer::firstPlay(const QString &isplay, int pos) {
    if (!mediaPlayer->isPlaying()) {
        qDebug("%s(%sPlayer%s)%s::%sReproduzindo um Arquivo Multimídia ...\033[0m", GRE, RED, GRE, RED, ORA);
        play(isplay, pos);
    }
}


/** Executa um item selecionado da playlist */
void OMPlayer::doubleplay(const QString &name) {
    qDebug("%s(%sPlayer%s)%s::%sReproduzindo um Arquivo Multimídia ...\033[0m", GRE, RED, GRE, RED, ORA);
    play(name, playlist->selectItems());
}


/** Função para selecionar aleatóriamente a próxima mídia */
void OMPlayer::nextRand() {
    qDebug("%s(%sPlayer%s)%s::%s Reproduzindo uma mídia aleatória ...\033[0m", GRE, RED, GRE, RED, ORA);
    if (listnum.size() < playlist->setListSize()) {
        actualitem = QRandomGenerator::global()->bounded(playlist->setListSize() - 1);

        /** No método aleatório convencional, alguns itens são reproduzidos mais vezes que os outros. Portanto,
         * criei esse método para executar os itens aleatoriamente só que um de cada vez. Assim, quando todos os
         * arquivos multimídia forem reproduzidos aleatoriamente, aí sim é zerado o contador para recameçar a
         * execução aleatória dos itens denovo. */

        while (!listnum.filter(QRegExp("^(" + QString::number(actualitem) + ")$")).isEmpty())
            actualitem = QRandomGenerator::global()->bounded(playlist->setListSize());

        listnum.append(QString::number(actualitem));
    } else {
        actualitem = QRandomGenerator::global()->bounded(playlist->setListSize());
        listnum.clear();
        listnum.append(QString::number(actualitem));
    }

    qDebug("%s(%sPlayer%s)%s::%sNúmero de itens reproduzidos aleatoriamente: %i ...\033[0m",
           GRE, RED, GRE, RED, BLU, listnum.size());
    play(playlist->getItems(actualitem), actualitem);
}


/** Botão next */
void OMPlayer::Next() {
    if (mediaPlayer->isPlaying() || playing) {

        /** Reprodução aleatória */
        if (randplay) {
            nextRand();
            return;
        }

        qDebug("%s(%sPlayer%s)%s::%sReproduzindo o próximo item ...\033[0m", GRE, RED, GRE, RED, ORA);
        play(playlist->getItems(nextitem), nextitem);
    }
}


/** Botão previous */
void OMPlayer::Previous() {
    if (mediaPlayer->isPlaying() || playing) {
        qDebug("%s(%sPlayer%s)%s::%sReproduzindo um item anterior ...\033[0m", GRE, RED, GRE, RED, ORA);
        play(playlist->getItems(previousitem), previousitem);
    }
}


/** Função para o botão de play/pause */
void OMPlayer::playPause() {
    if (!mediaPlayer->isPlaying()) {
        if (playlist->setListSize() > 0) {
            qDebug("%s(%sPlayer%s)%s::%sReproduzindo um Arquivo Multimídia ...\033[0m", GRE, RED, GRE, RED, ORA);
            if (playlist->getItems(actualitem).isEmpty()) actualitem = 0;
            play(playlist->getItems(actualitem), actualitem);
        }
        return;
    }
    mediaPlayer->pause(!mediaPlayer->isPaused());
}


/** Função para o botão stop */
void OMPlayer::setStop() {
    playing = false;
    actualitem = playlist->selectItems();
    mediaPlayer->stop();
    if (playlist->setListSize() == 0) listnum.clear();
    if(!prevent) onStop();
}


/** Função para alterar o botão play/pause */
void OMPlayer::onPaused(bool paused) {
    if (paused) {
        qDebug("%s(%sPlayer%s)%s::%sPausando ...\033[0m", GRE, RED, GRE, RED, ORA);
        Utils::changeIcon(playBtn, "play");
    } else {
        qDebug("%s(%sPlayer%s)%s::%sReproduzindo ...\033[0m", GRE, RED, GRE, RED, ORA);
        Utils::changeIcon(playBtn, "pause");
    }
}


/** Função que define alguns parâmetros ao iniciar a reprodução */
void OMPlayer::onStart() {
    playing = true;

    Utils::changeIcon(playBtn, "pause");
    stack->setCurrentIndex(1);
    slider->setDisabled(false);
    infoview->setFile(mediaPlayer->file());

    /** Definindo dimensões para o preview */
    Width = mediaPlayer->statistics().video_only.width;
    Height = mediaPlayer->statistics().video_only.height;


    /** Definindo o tempo de duração no slider */
    slider->setMinimum(int(mediaPlayer->mediaStartPosition()));
    slider->setMaximum(int(mediaPlayer->mediaStopPosition())/ unit);
    end->setText(QTime(0, 0, 0).addMSecs(
            int(mediaPlayer->mediaStopPosition())).toString(QString::fromLatin1("HH:mm:ss")));

    onTimeSliderLeave(IsPlay);
    changeIcons(IsPlay);
    updateChannelMenu();
    initAudioTrackMenu();
    updateSlider(mediaPlayer->position()); /** 00:00:00 */

    if (!powersaving) {
        screensaver->disable();
        powersaving = true;
    }
}


/** Função que define alguns parâmetros ao parar a reprodução */
void OMPlayer::onStop() {
    if (!playing) {
        qDebug("%s(%sPlayer%s)%s::%sFinalizando a Reprodução ...\033[0m", GRE, RED, GRE, RED, ORA);
        Utils::changeIcon(playBtn, "play");
        this->setWindowTitle(QString(PRG_NAME));

        stack->setCurrentIndex(0);
        slider->setMaximum(0);
        slider->setDisabled(true);
        current->setText("-- -- : -- --");
        end->setText("-- -- : -- --");
        infoview->resetValues();

        onTimeSliderLeave(IsPlay);
        changeIcons(IsPlay);
        updateChannelMenu();
        initAudioTrackMenu();

        if (powersaving) {
            screensaver->enable();
            powersaving = false;
        }
    }
}


/** Atualização dos itens da playlist */
void OMPlayer::changePlaylist(const QString &format, int duration) {
    if (playlist->setDuration() != 0) return;
    qDebug("%s(%sPlayer%s)%s::%sAtualizando %s ...\033[0m", GRE, RED, GRE, RED, UPD, STR(mediaPlayer->file()));

    int row = playlist->selectItems();
    playlist->updateItems(row, duration, format);
}


/** Ativação do modo de repetição da mídia */
void OMPlayer::setRepeat() {
    restart = false;
    JsonTools::boolJson("on_replay", false);

    if (mediaPlayer->repeat() == 0) {
        mediaPlayer->setRepeat(-1);
        Utils::changeIcon(replayBtn, "replay-one");
        if (!JsonTools::boolJson("on_replayone")) JsonTools::boolJson("on_replayone", true);
    } else {
        mediaPlayer->setRepeat(0);
        Utils::changeIcon(replayBtn, "replay");
        JsonTools::boolJson("on_replayone", false);
    }
}


/** Ativação do modo de repetição da playlist */
void OMPlayer::setReplay() {
    if (mediaPlayer->repeat() != 0) {
        setRepeat();
        return;
    }
    if (!restart) {
        restart = true;
        Utils::changeIcon(replayBtn, "replay-on");
        if (!JsonTools::boolJson("on_replay")) JsonTools::boolJson("on_replay", true);
    } else {
        restart = false;
        Utils::changeIcon(replayBtn, "replay");
        JsonTools::boolJson("on_replay", false);
    }
}


/** Ativação do modo aleatório */
void OMPlayer::setShuffle() {
    if (!randplay) {
        randplay = true;
        Utils::changeIcon(shuffleBtn, "shuffle-on");
        if (!JsonTools::boolJson("on_shuffle")) JsonTools::boolJson("on_shuffle", true);
    } else {
        randplay = false;
        Utils::changeIcon(shuffleBtn, "shuffle");
        JsonTools::boolJson("on_shuffle", false);
    }
}


/** Contador para mapear o clique único */
void OMPlayer::clickCount() {
    if (!click->isActive()) click->start();
    count++; /** Contador de cliques */
}


/** Função que mapeia um único clique e executa as ações de pausar e executar */
void OMPlayer::detectClick() {
    if (count == 1 && !enterpos && playing && pausing) playPause();
    pausing = false;
    count = 0;
}


/** Gerenciar tela cheia */
void OMPlayer::changeFullScreen() {
    count = 2;
    if (enterpos) return;
    if (this->isFullScreen()) leaveFullScreen();
    else enterFullScreen();
}


/** Entrar no modo tela cheia */
void OMPlayer::enterFullScreen() {
    qDebug("%s(%sInterface%s)%s::%sEntrando no Modo Tela Cheia ...\033[0m", GRE, RED, GRE, RED, ORA);

    if (this->isMaximized()) maximize = true; /** Mapear interface maximizada */
    this->showFullScreen();

    /** As vezes o fade nesse caso zica legal, melhor fechar direto */
    wctl->close();

    filter->setMove(false);
    enterpos = false;
}


/** Sair do modo tela cheia */
void OMPlayer::leaveFullScreen() {
    qDebug("%s(%sInterface%s)%s::%sSaindo do Modo Tela Cheia ...\033[0m", GRE, RED, GRE, RED, ORA);
    this->showNormal();

    /** Precisa caso a interface já estava maximizada antes. Note que showMaximized() só funciona após a execução
     * de showNormal(), a tela não pula direto de fulscreen para maximizado. */
    if (maximize) this->showMaximized();

    wctl->close(); /** a zica legal */
    filter->setMove(false);
    enterpos = maximize = false;
}


/** Efeito fade bacana para os controles e a playlist */
void OMPlayer::fadeWctl(OMPlayer::FADE option) {
    auto *animation = new QPropertyAnimation(effect, "opacity");
    auto *animation2 = new QPropertyAnimation(effect2, "opacity");
    animation->setDuration(FADE_VAL);
    animation2->setDuration(FADE_VAL);

    if (option == Show) {
        animation->setStartValue(0);
        animation->setEndValue(1);
        animation2->setStartValue(0);
        animation2->setEndValue(1);
        wctl->show();
        wctl->activateWindow();
    } else if (option == Hide) {
        animation->setStartValue(1);
        animation->setEndValue(0);
        animation2->setStartValue(1);
        animation2->setEndValue(0);

        connect(animation, &QPropertyAnimation::finished, [this](){
            wctl->close();
            if (!contmenu && !nomousehide) blankMouse();
            contmenu = nomousehide = false;
        });
    }
    animation->start();
    animation2->start();
}


/** Função auxiliar para fechar os controles e a playlist */
void OMPlayer::setHide() {
    /** Prevenindo a ocultação dos controles ao abrir o menu na playlist */
    if (control) {
        control = false;
        return;
    }
    fadeWctl(Hide);
    filter->setMove(false);
}


/** Função auxiliar para abrir os controles e a playlist */
void OMPlayer::setShow() {
    if (showsett) return;
    fadeWctl(Show);
}


/** Função que abre os diálogos */
void OMPlayer::setDialog(OMPlayer::DIALOG dialog) {
    showsett = nomousehide = true;
    arrowMouse();
    filter->setSett(showsett);
    this->setMaximumSize(size);
    this->setMinimumSize(size);

    if (dialog == AboutD) {
        qDebug("%s(%sInterface%s)%s::%sIniciando o diálogo sobre ...\033[0m", GRE, RED, GRE, RED, CYA);
        about->show();
        return;
    }

    if (dialog == SettingsD) {
        qDebug("%s(%sInterface%s)%s::%sIniciando o diálogo de configurações ...\033[0m", GRE, RED, GRE, RED, CYA);
        sett->show();
        return;
    }

    if (dialog == InfoD) {
        qDebug("%s(%sInterface%s)%s::%sIniciando o diálogo de informações ...\033[0m", GRE, RED, GRE, RED, CYA);
        playlist->hideFade();
        setHide();
        infoview->show();
    }
}


/** Função para executar operações ao fachar os diálogos */
void OMPlayer::closeDialog() {
    wctl->close();
    this->setMaximumSize(screen);
    this->setMinimumSize(min);
    this->activateWindow();
    this->setFocus();
    showsett = contmenu = nomousehide = control = false;
    filter->setMove(false);
    filter->setSett(showsett);
}


/** Ativar ocultação  */
void OMPlayer::hideTrue() {
    if (enterpos) {
        qDebug("%s(%sControls%s)%s::%sOcultação liberada ...\033[0m", GRE, RED, GRE, RED, SHW);
        enterpos = false;
    }
    filter->setFixed(enterpos);
}


/** Desativar ocultação */
void OMPlayer::hideFalse() {
    if (!enterpos) {
        qDebug("%s(%sControls%s)%s::%sOcultação impedida ...\033[0m", GRE, RED, GRE, RED, HID);
        enterpos = true;
    }
    filter->setFixed(enterpos);
}


/** Pré-visualização ao posicionar o mouse no slider */
void OMPlayer::onTimeSliderHover(int pos, int value) {
    int fixV = 72;
    QPoint gpos = QPoint(slider->pos().x(), this->height() - 105);

    /** Definição da posição na tela e exibição do tooltip */
    QPoint gpos1 = mapToGlobal(gpos + QPoint(pos, -30));
    QPoint gpos2 = mapToGlobal(gpos + QPoint(pos, -5));


    /** Arquivos de áudio não são vídeos */
    if (mediaPlayer->currentVideoStream() == (-1) || mediaPlayer->statistics().video.frame_rate == 0) return;
    int setX = Utils::calcX(fixV, Width, Height);

    /** Exibição da pré-visualização */
    preview->setFile(mediaPlayer->file());
    preview->setTimestamp(value * unit);
    preview->preview();
    preview->resize(setX, fixV);
    preview->move(gpos1 - QPoint(setX/2, fixV));
    pv->resize(setX + 20, fixV + 35);
    prev->resize(setX + 20, fixV + 35);
    prev->move(gpos2 - QPoint((setX + 20)/2, fixV + 35));
    ltime->setText(QTime(0, 0, 0).addMSecs(value * unit).toString(QString::fromLatin1("HH:mm:ss")));
    ltime->move(QPoint(setX/2 - 20, fixV + 10));

    if (!preview->isVisible()) QTimer::singleShot(500, this, [this](){ if (ispreview) {
        prev->show();
        preview->show();
    } });
    else if (ispreview) {
        prev->show();
        preview->show();
    }
}


/** Apenas para exibição do debug */
void OMPlayer::onTimeSliderEnter() {
    if (playing && mediaPlayer->statistics().video.frame_rate == 0)
        qDebug("%s(%sInterface%s)%s::%sExibindo a pré-visualização ...\033[0m", GRE, RED, GRE, RED, CYA);
    ispreview = true;
    hideFalse();
}


/** Saindo da pré-visualização */
void OMPlayer::onTimeSliderLeave(OMPlayer::STATUS status) {
    if (preview->isVisible()) {
        qDebug("%s(%sInterface%s)%s::%sFinalizando a pré-visualização ...\033[0m", GRE, RED, GRE, RED, CYA);
        preview->close();
        prev->close();
    }
    if (status == Default) ispreview = false;
}


/** Altera o tempo de execução da barra de progresso de execução */
void OMPlayer::seekBySlider(int value) {
    if (mediaPlayer->isPaused() && mediaPlayer->currentVideoStream() > (-1) &&
        mediaPlayer->statistics().video.frame_rate == 0) return;
    mediaPlayer->setSeekType(QtAV::AccurateSeek);
    mediaPlayer->seek(qint64(value) * unit);
}


/** Ação ao terminar a atualização do slider */
void OMPlayer::seekFinished(qint64 pos) {
    qDebug("%s(%sPlayer%s)%s::%sAtualizando posição de execução %lld / %lld ...\033[0m",
           GRE, RED, GRE, RED, ORA, pos, mediaPlayer->duration());
}


/** Função para deixar o reprodutor no mudo */
void OMPlayer::setMute() {
    if (playing && !mediaPlayer->statistics().audio.available) return;
    if (muted) {
        muted = false;
        setVolume(volume->value());
    } else {
        setVolume(0);
        muted = true;
    }
}


/** Altera o volume do reprodutor ao pressionar ou mover a barra de volume */
void OMPlayer::setVolume(int value) {
    /** O volume do reprodutor é definido com um valor do tipo double que vai de 0.0 a 2.0, portanto o valor
     * do tipo int a ser recebido, precisa ser reajustado e convertido. */
    mediaPlayer->audio()->setVolume((double)value / 100);
}


/** Ação ao terminar a atualização do volume */
void OMPlayer::volumeFinished(qreal pos) {
    int value = int(pos * 100);
    qDebug("%s(%sPlayer%s)%s::%sAlterando o volume para %i ...\033[0m", GRE, RED, GRE, RED, ORA, value);
    muted = false;

    if (JsonTools::floatJson("volume") != pos) JsonTools::floatJson("volume", (float)pos);

    if (playing && !mediaPlayer->statistics().audio.available) {
        Utils::changeIcon(volumeBtn, "nosound");
        return;
    }

    if (value == 0) {
        Utils::changeIcon(volumeBtn, "mute");
        muted = true;
    } else if (value > 0 && value <= 25) Utils::changeIcon(volumeBtn, "volume_low");
    else if (value > 25 && value <= 75) Utils::changeIcon(volumeBtn, "volume_medium");
    else if (value > 75) Utils::changeIcon(volumeBtn, "volume_high");
}


/** Pré-visualização ao posicionar o mouse na barra de volume */
void OMPlayer::onTimeVolume(int pos, int value) {
    string str;
    if (value < 10 && value > 0) str = "0";
    QString text = QString::asprintf(" %s%i ", str.c_str(), value);

    /** Definição da posição na tela e exibição do tooltip */
    if (QString::compare(lvol->text(), text) != 0) {
        QPoint point = QPoint(volume->pos().x(), this->height() - volume->pos().y());
        QPoint gpos = mapToGlobal(point + QPoint(pos - 18, -24));
        lvol->setText(text);
        wvol->move(gpos);
        wvol->show();
    }
}


/** Função para alterar o canal de áudio no menu */
void OMPlayer::updateChannelMenu() {
    if (channelAction) channelAction->setChecked(false);

    QtAV::AudioOutput *ao = mediaPlayer ? mediaPlayer->audio() : nullptr;
    if (!ao) return;

    QtAV::AudioFormat::ChannelLayout cl = ao->audioFormat().channelLayout();
    QList<QAction*> as = channel->actions();

    /** Buscando e alterando o canal no menu */
    foreach (QAction *action, as) {
        if (action->data().toInt() != (int)cl) continue;
        if (playing) action->setChecked(true);
        else action->setChecked(false);
        channelAction = action;
        break;
    }

    /** Zerando canal de áudio salvo ao parar o reprodutor */
    if (!playing) channelAction = nullptr;
}


/** Função para alterar o canal de áudio durante a reprodução da mídia */
void OMPlayer::changeChannel(QAction *action) {
    if (action == channelAction) return;
    if (!playing) {
        action->toggle();
        return;
    }

    /** Verificando existência de áudio */
    QtAV::AudioFormat::ChannelLayout cl = (QtAV::AudioFormat::ChannelLayout)action->data().toInt();
    QtAV::AudioOutput *ao = mediaPlayer ? mediaPlayer->audio() : nullptr;
    if (!ao) return;

    channelAction->setChecked(false);
    channelAction = action;
    channelAction->setChecked(true);

    if (!ao->close()) {
        qDebug("%s(%sPlayer%s)%s::%sFalha ao fechar o áudio ...", GRE, RED, GRE, RED, ERR);
        return;
    }

    /** Alterando o formato */
    QtAV::AudioFormat af(ao->audioFormat());
    af.setChannelLayout(cl);
    ao->setAudioFormat(af);

    if (!ao->open()) qDebug("%s(%sPlayer%s)%s::%sFalha ao abrir o áudio ...", GRE, RED, GRE, RED, ERR);
}


/** Função para definir as saídas de áudio */
void OMPlayer::initAudioTrackMenu() {
    QAction *a{};
    QList<QAction*> as = audiotrack->actions();
    int track = mediaPlayer->currentAudioStream();
    int tracks = mediaPlayer->audioStreamCount();

    /** Redefinindo o menu de contexto */
    while (tracks < as.count()) {
        a = as.takeLast();
        ta->removeAction(a);
        delete a;
    }

    if (as.count() == 0 || tracks == 0) {
        /** Definindo o menu de contexto na inicialização */
        a = audiotrack->addAction(tr("No sound"));
        a->setData(as.size());
        a->setCheckable(false);
        a->setChecked(false);
        audiotrack->addAction(a);
        return;
    } else {
        /** Precisa para tirar o menu de sem som */
        a = as.takeLast();
        audiotrack->removeAction(a);
        delete a;
    }

    /** Redefinindo o menu de contexto ao parar */
    if (!playing) {
        a = audiotrack->addAction(tr("No sound"));
        a->setData(as.size());
        a->setCheckable(false);
        a->setChecked(false);
        audiotrack->addAction(a);
        return;
    }

    /** Adicionando saídas para o menu de contexto */
    while (tracks > as.size()) {
        a = audiotrack->addAction(tr("Track") + " " + QString::number(as.size()));
        a->setData(as.size());
        a->setCheckable(true);
        a->setChecked(false);
        ta->addAction(a);
        as.push_back(a);
    }

    /** Setando a primeira opção */
    trackAction = as.first();
    trackAction->setChecked(true);
}


/** Função para alterar canais de áudio */
void OMPlayer::changeAudioTrack(QAction *action){
    int track = action->data().toInt();
    if (trackAction == action) return;

    /** Verificando e mudando o canal de áudio */
    if (!mediaPlayer->setAudioStream(track)) {
        action->toggle();
        return;
    }

    if (trackAction) trackAction->setChecked(false);
    trackAction = action;
    trackAction->setChecked(true);
}


/** Função para atualizar a barra de progresso de execução */
void OMPlayer::updateSlider(qint64 value) {
    if (mediaPlayer->isSeekable()) slider->setValue(int(value / unit));
    current->setText(QTime(0, 0, 0).addMSecs(int(value)).toString(QString::fromLatin1("HH:mm:ss")));
}


/** Função para atualizar a barra de progresso de execução */
void OMPlayer::updateSliderUnit() {
    unit = mediaPlayer->notifyInterval();
    slider->setMaximum(int(mediaPlayer->mediaStopPosition())/ unit);
    updateSlider(mediaPlayer->position());
}


/** Função para recarregar os ícones do programa */
void OMPlayer::changeIcons(OMPlayer::STATUS change) {
    int value = int(mediaPlayer->audio()->volume() * 100);

    if (playing && !mediaPlayer->statistics().audio.available) Utils::changeIcon(volumeBtn, "nosound");
    else if (value > 0 && value <= 25) Utils::changeIcon(volumeBtn, "volume_low");
    else if (value > 25 && value <= 75) Utils::changeIcon(volumeBtn, "volume_medium");
    else if (value > 75) Utils::changeIcon(volumeBtn, "volume_high");
    else if (value == 0) Utils::changeIcon(volumeBtn, "mute");

    if (change != IsPlay) {
        if (mediaPlayer->isPlaying()) Utils::changeIcon(playBtn, "pause");
        else Utils::changeIcon(playBtn, "play");

        if (restart) Utils::changeIcon(replayBtn, "replay-on");
        else if (mediaPlayer->repeat() != 0) Utils::changeIcon(replayBtn, "replay-one");
        else Utils::changeIcon(replayBtn, "replay");
        if (randplay) Utils::changeIcon(shuffleBtn, "shuffle-on");
        else Utils::changeIcon(shuffleBtn, "shuffle");

        Utils::changeIcon(stopBtn, "stop");
        Utils::changeIcon(nextBtn, "next");
        Utils::changeIcon(previousBtn, "previous");

        nextBtn->secondIcon("next", "forward");
        previousBtn->secondIcon("previous", "backward");
        replayBtn->secondIcon("replay", "replay-one");

        /** Alterando os ícones dos diálogos */
        playlist->changeIcons();
        infoview->changeIcons();
        about->changeIcons();

        Utils::changeMenuIcon(channel, "channel");
        Utils::changeMenuIcon(aspectratio, "aspectratio");
        Utils::changeMenuIcon(speed, "speed");
        Utils::changeMenuIcon(audiotrack, "track");
    }
}


/** Executando o subtítulo */
void OMPlayer::runSubtitle() {
    /** Além de ser ruim, o subtítulo vive dando falha de segmentação se executar mais de uma música com legenda.
    * Então, só reconstruindo o filtro do subtítulo pra resolver. */
    if (subtitle) subtitle->uninstall();
    delete subtitle;
    subtitle = new SubtitleFilter(this);
    subtitle->setPlayer(mediaPlayer);
    subtitle->installTo(video);
    if (!JsonTools::boolJson("sub_enable")) enableSubtitle(false);
    subtitle->setAutoLoad(JsonTools::boolJson("sub_autoload"));
    subtitle->setCodec(QByteArray::fromStdString(JsonTools::stringJson("sub_charset").toStdString()));
    subtitle->setEngines({JsonTools::stringJson("sub_engine")});

    /** Setando uma fonte para a legenda se possível */
    if (QString::compare(JsonTools::stringJson("sub_font"), "undefined") != 0) {
        QFont font;
        font.fromString(JsonTools::stringJson("sub_font"));
        font.setHintingPreference(QFont::PreferFullHinting);
        font.setStyleStrategy(static_cast<QFont::StyleStrategy>(QFont::PreferAntialias | QFont::PreferQuality));
        subtitle->setFont(font);
    }

    /** Setando uma cor para a legenda se possível */
    if (QString::compare(JsonTools::stringJson("sub_color"), "undefined") != 0) {
        QStringList s{JsonTools::stringJson("sub_color").split("-")};
        subtitle->setColor(QColor(s[0].toInt(), s[1].toInt(), s[2].toInt()));
    }
}


/** Retorna uma lista de todas as legendas encontradas para a mídia atual */
QString OMPlayer::mapFirstSubtitle() {
    QFileInfo file{mediaPlayer->file()};
    QString base{mediaPlayer->file().remove(file.suffix())};
    Extensions e;
    foreach(const QString s, e.subtitles()) if (QFileInfo::exists(base + s)) return base + s;
    return {};
}


/** Ativação da legenda */
void OMPlayer::enableSubtitle(bool val) {
    if (playing) {
        if (val) {
            subtitle->installTo(video);
            mediaPlayer->setRenderer(video);
        } else subtitle->uninstall();
    }
}


/** Definindo o conjunto de caracteres da legenda */
void OMPlayer::charsetSubtitle() {
    if (playing) {
        runSubtitle();
        if (!mapFirstSubtitle().isEmpty()) subtitle->setFile(mapFirstSubtitle());
    }
}


/** Função para mapear o status de um arquivo multimídia */
void OMPlayer::onMediaStatusChanged() {
    QString status;
    auto *player = reinterpret_cast<QtAV::AVPlayer*>(sender());
    if (!player) return;

    switch (player->mediaStatus()) {
        case QtAV::InvalidMedia:
            invalid = true;
            break;
        case QtAV::BufferedMedia:
            status = "Buffered !";
            break;
        case QtAV::LoadedMedia:
            status = "Loaded !";
            break;
        case 7:
            status = "Finished !";

            /** Esse status só é emitido se o arquivo multimídia for reproduzido por completo. Portanto, ele pode ser
             * usado para o next automático. A emissão feita pelo reprodutor deve ser bloqueada nesse caso. */

            qDebug("%s(%sPlayer%s)%s::%sNext automático ...\033[0m", GRE, RED, GRE, RED, ORA);
            if (mediaPlayer->repeat() != 0) return;
            if (actualitem == playlist->setListSize() - 1 && !restart && !randplay) {
                playing = false;
                playlist->selectClean();
                onStop();
                previousitem = playlist->setListSize() - 1;
                actualitem = 0;
                nextitem = 1;
            } else {
                prevent = true;
                Next();
            }

            break;
        default:
            qDebug("%s(%sPlayer%s)%s::%sStatus não mapeado:%s %i\033[0m",
                   GRE, RED, GRE, RED, YEL, ERR, player->mediaStatus());
            break;
    }

    if (!status.isEmpty())
        qDebug("%s(%sPlayer%s)%s::%sStatus do Arquivo Multimídia %s\033[0m", GRE, RED, GRE, RED, SND, STR(status));
}


/** Debug em caso de eventuais erros */
void OMPlayer::handleError(const QtAV::AVError &error) {
    string txt = error.string().toStdString();
    Utils::rm_nl(txt);
    qDebug("%s(%sAVError%s)%s::%s %s", GRE, RED, GRE, RED, ERR, txt.c_str());

    #if defined(Q_OS_LINUX)
        /** Notificações de erro */
        QStringList err = error.string().split('\n');
        err.append(Utils::mediaTitle(mediaPlayer->file()));
        err.append(err[0][0].toUpper());
        err[2].append("\n" + err[1]);
        err[3].append(err[0].remove(0, 1));
        notify_send(err[3].toStdString().c_str(), err[2].toStdString().c_str());
    #endif

    if (!QFileInfo::exists(mediaPlayer->file()) || invalid) {
        if (actualitem == playlist->setListSize() - 1 && !restart && !randplay) {
            playing = false;
            onStop();
            playlist->selectClean();
            previousitem = playlist->setListSize() - 1;
            actualitem = 0;
            nextitem = 1;
        } else {
            playing = true;
            Next();
        }
        invalid = false;
    }
}


/**********************************************************************************************************************/


/** Evento que fornece a posição da interface pai para posicionar corretamente os controles */
void OMPlayer::moveEvent(QMoveEvent *event) {
    wctl->move(event->pos());
    QWidget::moveEvent(event);
}


/** Evento que mapeia o redirecionamento da interface para o ajuste dos controles */
void OMPlayer::resizeEvent(QResizeEvent *event) {
    size = event->size();
    wctl->setFixedSize(size);
    QWidget::resizeEvent(event);
}


/** Mapeamento dos eventos de maximizar, minimizar e restaurar */
void OMPlayer::changeEvent(QEvent *event) {
    if(event->type() == QEvent::WindowStateChange) {
        if (this->windowState() == Qt::WindowNoState) {
            qDebug("%s(%sInterface%s)%s::%sRestaurando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, RED, ORA);
            JsonTools::boolJson("maximized", false);
        }
        else if (this->windowState() == Qt::WindowMinimized) {
            qDebug("%s(%sInterface%s)%s::%sMinimizando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, RED, ORA);
            if (showsett) this->showNormal();
        }
        else if (this->windowState() == Qt::WindowMaximized) {
            qDebug("%s(%sInterface%s)%s::%sMaximizando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, RED, ORA);
            JsonTools::boolJson("maximized", true);
            playlist->hideFade();

        }
        else if (this->windowState() == (Qt::WindowMinimized|Qt::WindowMaximized)) {
            qDebug("%s(%sInterface%s)%s::%s Minimizando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, RED, ORA);
            if (showsett) this->showNormal();
        }
    }
}


/** Ação ao fechar o programa */
void OMPlayer::closeEvent(QCloseEvent *event) {
    if (subtitle) subtitle->uninstall();
    delete subtitle;
    screensaver->enable();
    playlist->save();
    qDebug("%s(%sInterface%s)%s::%sFinalizando o Reprodutor Multimídia !\033[0m", GRE, RED, GRE, RED, CYA);
    event->accept();
}


/**********************************************************************************************************************/


/** Função para o menu de contexto do programa */
void OMPlayer::ShowContextMenu(const QPoint &pos) {
    if (listmenu || !enterpos) {
        qDebug("%s(%sInterface%s)%s::%sIniciando o Menu de Contexto ...\033[0m", GRE, RED, GRE, RED, CYA);
        arrowMouse();
    }

    contmenu = true;
    auto *contextMenu = new CMenu(nullptr, this);

    if (listmenu && enterpos) {
        control = true;
        contextMenu->setOpacity(0.9);

        /** Menu de informação de mídia */
        QAction mediainfo(tr("Multimedia Info"), this);
        Utils::changeMenuIcon(mediainfo, "about");
        connect(&mediainfo, &QAction::triggered, [this](){ setDialog(DIALOG::InfoD); });

        QAction saveplaylist(tr("Save Playlist"), this);

//        contextMenu->addAction(&saveplaylist);
        contextMenu->addAction(&mediainfo);
        contextMenu->exec(mapToGlobal(pos));
        return;
    }

    if (!enterpos) {
        /** Opções de menu*/
        auto *other = new CMenu(tr("Other Options"), this);
        auto *visualization = new CMenu(tr("Visualizations"), this);
        auto *optionVideo = new CMenu(tr("Video Options"), this);

        /** Modo repetição */
        QAction repeat(tr("Repeat Mode"), this);
        repeat.setShortcut(QKeySequence(CTRL | ALT | Qt::Key_T));
        Utils::changeMenuIcon(repeat, "replay-one-menu");
        connect(&repeat, SIGNAL(triggered()), SLOT(setRepeat()));

        /** Dimensão de exibição */
        QAction rotation(tr("Rotation"), this);

        /** Menu de abrir */
        QAction open(tr("Open Files"), this);
        open.setShortcut(QKeySequence(CTRL | Qt::Key_O));
        Utils::changeMenuIcon(open, "folder");
        connect(&open, SIGNAL(triggered()), SLOT(openMedia()));

        /** Menu tela cheia */
        QAction fullscreen(tr("Show Fullscreen"), this);
        if (this->isFullScreen())
            fullscreen.setText(tr("Exit Fullscreen"));
        fullscreen.setShortcut(QKeySequence(ALT | Qt::Key_Enter));
        Utils::changeMenuIcon(fullscreen, "fullscreen");
        connect(&fullscreen, SIGNAL(triggered()), SLOT(changeFullScreen()));

        /** Menu aleatório */
        QAction shuffle(tr("Shuffle"), this);
        shuffle.setShortcut(QKeySequence(CTRL | Qt::Key_H));
        Utils::changeMenuIcon(shuffle, "shuffle-menu");
        connect(&shuffle, SIGNAL(triggered()), SLOT(setShuffle()));

        /** Menu replay */
        QAction replay(tr("Replay"), this);
        replay.setShortcut(QKeySequence(CTRL | Qt::Key_T));
        Utils::changeMenuIcon(replay, "replay-menu");
        connect(&replay, SIGNAL(triggered()), SLOT(setReplay()));

        /** Menu de configuração */
        QAction settings(tr("Settings"), this);
        settings.setShortcut(QKeySequence(ALT | Qt::Key_S));
        Utils::changeMenuIcon(settings, "settings");
        connect(&settings, &QAction::triggered, [this](){ setDialog(SettingsD); });

        /** Menu sobre */
        QAction mabout(tr("About"), this);
        Utils::changeMenuIcon(mabout, "about");
        connect(&mabout, &QAction::triggered, [this](){ setDialog(AboutD); });

        /** Montagem do menu de opções de vídeo */
        optionVideo->addMenu(aspectratio);
//        optionVideo->addAction(&rotation);

        /** Montagem do menu para outras opções */
        other->addMenu(audiotrack);
        other->addMenu(channel);
        other->addMenu(speed);
        other->addAction(&repeat);

        /** Montagem do menu principal */
        contextMenu->addAction(&open);
        contextMenu->addSeparator();
        contextMenu->addAction(&fullscreen);
        contextMenu->addSeparator();
        contextMenu->addAction(&shuffle);
        contextMenu->addAction(&replay);
        contextMenu->addSeparator();
//        contextMenu->addMenu(visualization);
        contextMenu->addMenu(optionVideo);
        contextMenu->addMenu(other);
        contextMenu->addSeparator();
        contextMenu->addAction(&settings);
        contextMenu->addAction(&mabout);
        contextMenu->exec(mapToGlobal(pos));
    }
}
