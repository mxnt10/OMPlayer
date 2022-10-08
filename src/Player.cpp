#include <QMenu>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QWidgetAction>
#include <ClickableMenu>
#include <Utils>

#include "Player.h"

#define ClickableMenu(x) ClickableMenu(x + "\t\t")
#define TR(x) ("  " + tr(x))
#define ACTION(x) addAction("  " + x)


/**********************************************************************************************************************/


/** Construtor que define a interface do programa */
OMPlayer::OMPlayer(QWidget *parent) : QWidget(parent) {
    qDebug("%s(%sInterface%s)%s::%sIniciando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, RED, CYA);
    this->setWindowTitle(QString(PRG_NAME));
    this->setWindowIcon(QIcon(Utils::setIcon()));
    this->setMinimumSize(min);
    this->setMouseTracking(true);
    this->setAutoFillBackground(true);
    this->move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center());
    double vol = JsonTools::floatJson("volume");


    /** Fundo preto */
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::black);
    this->setPalette(pal);


    /** Preview do reprodutor */
    prev = new QWidget();
    prev->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    prev->setAttribute(Qt::WA_NoSystemBackground, true);
    prev->setAttribute(Qt::WA_TranslucentBackground, true);
    pv = new QWidget(prev);
    pv->setStyleSheet(Utils::setStyle("widget"));
    preview = new VideoPreviewWidget(pv);
    preview->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    ltime = new QLabel(pv);
    ltime->setAttribute(Qt::WA_NoSystemBackground, true);
    ltime->setAttribute(Qt::WA_TranslucentBackground, true);
    wvol = new QWidget();
    wvol->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    wvol->setAttribute(Qt::WA_NoSystemBackground, true);
    wvol->setAttribute(Qt::WA_TranslucentBackground, true);
    lvol = new Label(CENTER, 42, 22, wvol);
    lvol->setStyleSheet(Utils::setStyle("label"));

    /** Janelas de configurações do programa, informações e demais */
    about = new About(this);
    sett = new Settings(this);
    infoview = new StatisticsView(this);
    screensaver = new ScreenSaver(this);
    connect(sett, &Settings::changethemeicon, [this](){ changeIcons(); });
    connect(sett, &Settings::emitvalue, this, &OMPlayer::setRenderer);
    connect(sett, &Settings::emitclose, this, &OMPlayer::closeDialog);
    connect(infoview, &StatisticsView::emitclose, this, &OMPlayer::closeDialog);
    connect(about, &About::emitclose, this, &OMPlayer::closeDialog);


    /** Parte principal do programa que permite o funcionamento do reprodutor */
    mediaPlayer = new AVPlayer(this);
    connect(mediaPlayer->audio(), &QtAV::AudioOutput::volumeChanged, this, &OMPlayer::volumeFinished);
    connect(mediaPlayer, &AVPlayer::mediaStatusChanged, this, &OMPlayer::onMediaStatusChanged);
    connect(mediaPlayer, &AVPlayer::seekFinished, this, &OMPlayer::seekFinished);
    connect(mediaPlayer, &AVPlayer::positionChanged, this, &OMPlayer::updateSlider);
    connect(mediaPlayer, &AVPlayer::notifyIntervalChanged, this, &OMPlayer::updateSliderUnit);
    connect(mediaPlayer, &AVPlayer::started, this, &OMPlayer::onStart);
    connect(mediaPlayer, &AVPlayer::stopped, this, &OMPlayer::onStop);
    connect(mediaPlayer, &AVPlayer::paused, this, &OMPlayer::onPaused);
    connect(mediaPlayer, &AVPlayer::error, this, &OMPlayer::handleError);


    /** Playlist do reprodutor */
    playlist = new PlayList();
    setTotalItems();
    connect(playlist, &PlayList::aboutToPlay, this, &OMPlayer::doubleplay);
    connect(playlist, &PlayList::firstPlay, this, &OMPlayer::firstPlay);
    connect(playlist, &PlayList::selected, this, &OMPlayer::setSelect);
    connect(playlist, &PlayList::emitremove, this, &OMPlayer::ajustActualItem);
    connect(playlist, &PlayList::emithide, this, &OMPlayer::hideTrue);
    connect(playlist, &PlayList::emitnohide, this, &OMPlayer::hideFalse);
    connect(playlist, &PlayList::enterListView, this, &OMPlayer::enterList);
    connect(playlist, &PlayList::leaveListView, this, &OMPlayer::leaveList);
    connect(playlist, &PlayList::emitstop, this, &OMPlayer::setStop);
    connect(playlist, &PlayList::emitItems, this, &OMPlayer::setTotalItems);
    connect(playlist, &PlayList::setcontmenu, [this](){ contmenu = true; });


    /** Barra de progresso de execução e Labels */
    current = new Label(CENTER, 70, "-- -- : -- --");
    end = new Label(CENTER, 70, "-- -- : -- --");
    slider = new Slider(this, true, (-1), 28, 0);
    connect(slider, &Slider::onHover, this, &OMPlayer::onTimeSliderHover);
    connect(slider, &Slider::sliderMoved, this, &OMPlayer::seekBySlider);
    connect(slider, &Slider::emitEnter, this, &OMPlayer::onTimeSliderEnter);
    connect(slider, &Slider::emitLeave, [this](){ onTimeSliderLeave(); });
    connect(slider, &Slider::sliderPressed, [this](){ seekBySlider(slider->value()); });


    /** Botões para os controles de reprodução */
    playBtn = new Button(Button::button, "play", 48, tr("Play"));
    stopBtn = new Button(Button::button, "stop", 32, tr("Stop"));
    nextBtn = new Button(Button::button, "next", 32, tr("Next"));
    previousBtn = new Button(Button::button, "previous", 32, tr("Previous"));
    replayBtn = new Button(Button::button, "replay", 32, tr("Replay"));
    shuffleBtn = new Button(Button::button, "shuffle", 32, tr("Shuffle"));
    volumeBtn = new Button(Button::button, "volume_high", 32, tr("Volume") + ": " + QString::number(vol * 100));
    connect(playBtn, &Button::clicked, this, &OMPlayer::playPause);
    connect(stopBtn, &Button::clicked, this, &OMPlayer::setStop);
    connect(nextBtn, &Button::clicked, this, &OMPlayer::Next);
    connect(previousBtn, &Button::clicked, this, &OMPlayer::Previous);
    connect(replayBtn, &Button::clicked, this, &OMPlayer::setReplay);
    connect(shuffleBtn, &Button::clicked, this, &OMPlayer::setShuffle);
    connect(volumeBtn, &Button::clicked, this, &OMPlayer::setMute);


    /** Verificando se as opções de replay e aleatório estão ativas */
    if (JsonTools::boolJson("on_replay")) setReplay();
    if (JsonTools::boolJson("on_shuffle")) setShuffle();


    /** Controle do volume */
    volume = new Slider(this, false, 90, (-1), 100, "volume");
    volume->setValue(int(vol * 100));
    mediaPlayer->audio()->setVolume(vol);
    connect(volume, &Slider::onHover, this, &OMPlayer::onTimeVolume);
    connect(volume, &Slider::sliderMoved, this, &OMPlayer::setVolume);
    connect(volume, &Slider::sliderPressed, [this](){ setVolume(volume->value()); });
    connect(volume, &Slider::emitLeave, [this](){ wvol->close(); });


    /** Plano de fundo semitransparente dos controles de reprodução */
    qDebug("%s(%sInterface%s)%s::%sPreparando o layout da interface ...\033[0m", GRE, RED, GRE, RED, CYA);
    auto *bgcontrol = new QWidget();
    bgcontrol->setMaximumHeight(120);
    bgcontrol->setStyleSheet(Utils::setStyle("widget"));


    /** Ajuste do plano de fundo dos controles */
    auto *bgctl = new QGridLayout();
    bgctl->setMargin(10);
    bgctl->addWidget(bgcontrol, 0, 0);


    /** Gerando uma linha */
    auto line = new QFrame();
    line->setFixedWidth(2);
    line->setFixedHeight(25);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background: #cccccc");


    /** Layout dos botões */
    auto *buttons = new QHBoxLayout();
    buttons->setSpacing(5);
    buttons->addStretch(1);
    buttons->addSpacing(30);
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
    buttons->addWidget(volume);
    buttons->addStretch(1);


    /** Ajustes na barra de execução para inserir o tempo de execução e a duração */
    auto *fgslider = new QHBoxLayout();
    fgslider->addWidget(current);
    fgslider->addWidget(slider);
    fgslider->addWidget(end);


    /** Conteiner dos controles */
    auto *fctl = new QWidget();
    auto *ffilter = new EventFilter(this, EventFilter::Control);
    fctl->installEventFilter(ffilter);
    connect(ffilter, &EventFilter::emitLeave, this, &OMPlayer::hideTrue);
    connect(ffilter, &EventFilter::emitEnter, this, &OMPlayer::hideFalse);


    /** Ajuste dos controles */
    auto *fgctl = new QVBoxLayout(fctl);
    fgctl->setContentsMargins(10, 12, 10, 22);
    fgctl->addLayout(fgslider);
    fgctl->addLayout(buttons);


    /** Widget separado do pai para os controles e a playlist */
    wctl = new QWidget(this);
    wctl->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    wctl->setAttribute(Qt::WA_NoSystemBackground, true);
    wctl->setAttribute(Qt::WA_TranslucentBackground, true);


    /** Layout dos controles de reprodução */
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


    /** Definição da logo */
    auto *wlogo = new QWidget();
    wlogo->setMouseTracking(true);
    logo = new Label(CENTER, 0, 0);
    logo->setPixmap(QPixmap(Utils::setIcon(Utils::Logo)));


    /** Layout da logo */
    wlayout = new QWidget();
    wlayout->setMouseTracking(true);
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
    layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->addWidget(stack);
    this->setLayout(layout);


    /** Filtro de eventos */
    filter = new EventFilter(this, EventFilter::General);
    auto *cfilter = new EventFilter(this, EventFilter::Control);
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
    connect(filter, &EventFilter::emitSettings, [this](){ setDialog(SettingsD); });
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
    auto *subMenu = new ClickableMenu(TR("Channel"));
    channel = new QMenu(this);
    channel = subMenu;
    connect(subMenu, &ClickableMenu::triggered, this, &OMPlayer::changeChannel);
    subMenu->addAction(TR("As input"))->setData(AudioFormat::ChannelLayout_Unsupported);
    subMenu->addAction(TR("Stereo"))->setData(AudioFormat::ChannelLayout_Stereo);
    subMenu->addAction(TR("Mono (Center)"))->setData(AudioFormat::ChannelLayout_Center);
    subMenu->addAction(TR("Left"))->setData(AudioFormat::ChannelLayout_Left);
    subMenu->addAction(TR("Right"))->setData(AudioFormat::ChannelLayout_Right);
    auto *ag = new QActionGroup(subMenu);
    ag->setExclusive(true);


    /** Verificando o que foi clicado */
    foreach(QAction* action, subMenu->actions()) {
        ag->addAction(action);
        action->setCheckable(true);
    }


    /** Menu dos dimensões de tela para o menu de contexto */
    subMenu = new ClickableMenu(TR("AspectRatio"));
    aspectratio = new QMenu(this);
    aspectratio = subMenu;
    aspectAction = subMenu->ACTION(Utils::aspectStr(Utils::AspectVideo));
    aspectAction->setData(Utils::aspectNum(Utils::AspectVideo));
    connect(subMenu, &ClickableMenu::triggered, this, &OMPlayer::switchAspectRatio);
    subMenu->ACTION(Utils::aspectStr(Utils::AspectAuto ))->setData(Utils::AspectAuto);
    subMenu->ACTION(Utils::aspectStr(Utils::Aspect11   ))->setData(Utils::Aspect11);
    subMenu->ACTION(Utils::aspectStr(Utils::Aspect21   ))->setData(Utils::Aspect21);
    subMenu->ACTION(Utils::aspectStr(Utils::Aspect32   ))->setData(Utils::Aspect32);
    subMenu->ACTION(Utils::aspectStr(Utils::Aspect43   ))->setData(Utils::Aspect43);
    subMenu->ACTION(Utils::aspectStr(Utils::Aspect54   ))->setData(Utils::Aspect54);
    subMenu->ACTION(Utils::aspectStr(Utils::Aspect118  ))->setData(Utils::Aspect118);
    subMenu->ACTION(Utils::aspectStr(Utils::Aspect149  ))->setData(Utils::Aspect149);
    subMenu->ACTION(Utils::aspectStr(Utils::Aspect1410 ))->setData(Utils::Aspect1410);
    subMenu->ACTION(Utils::aspectStr(Utils::Aspect169  ))->setData(Utils::Aspect169);
    subMenu->ACTION(Utils::aspectStr(Utils::Aspect1610 ))->setData(Utils::Aspect1610);
    subMenu->ACTION(Utils::aspectStr(Utils::Aspect235  ))->setData(Utils::Aspect235);
    ag = new QActionGroup(subMenu);
    ag->setExclusive(true);


    /** Verificando o que foi clicado */
    foreach(QAction* action, subMenu->actions()) {
        ag->addAction(action);
        action->setCheckable(true);
    }
    aspectAction->setChecked(true);


    /** Menu de velocidade para o menu de contexto */
    subMenu = new ClickableMenu(TR("Speed"));
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
}


/** Destrutor */
OMPlayer::~OMPlayer() = default;


/**********************************************************************************************************************/


/** Função para abrir arquivos multimídia */
void OMPlayer::openMedia(const QStringList &parms) { playlist->addItems(parms); }


/** Rodando as funções após aberto outras instâncias */
void OMPlayer::onLoad() {
    playlist->load(PlayList::Second);
    if (totalitems < playlist->setListSize()) {
        if (!playing) play(playlist->getItems(totalitems), totalitems);
        else playlist->selectCurrent(actualitem);
        setTotalItems();
    }
}


/** Setando opções de Rendenização */
void OMPlayer::setRenderer(const QString &op) {
    if (stack->count() == 2) stack->removeWidget(video->widget());

    /** Para mudar a renderização é necessário recriar o widget removido para depois ser readicionado.
     * O uso do QStackWidget vai garantir que o widget vai ser exibido no lugar da logo sem problemas. */

    struct {
        const char* name;
        VideoRendererId id;
    } rend[] = {{"OpenGL",     VideoRendererId_OpenGLWidget },
                {"QGLWidget2", VideoRendererId_GLWidget2    },
                {"Direct2D",   VideoRendererId_Direct2D     },
                {"GDI",        VideoRendererId_GDI          },
                {"XVideo",     VideoRendererId_XV           },
                {"X11",        VideoRendererId_X11          },
                {"QGLWidget",  VideoRendererId_GLWidget     },
                {"Widget",     VideoRendererId_Widget       }, {nullptr, 0}};

    for (int i = 0; rend[i].name; ++i) {
        if (QString::compare(op, rend[i].name, Qt::CaseSensitive) == 0) {
            video = VideoRenderer::create(rend[i].id);
            video->widget()->setMouseTracking(true);
            break;
        }
    }

    /** Ao recriar o widget, o índice do QStackWidget precisará ser definido novamente */
    if (!video && !video->isAvailable()) video = new VideoOutput(this);
    mediaPlayer->setRenderer(video);
    stack->addWidget(video->widget());
    if (playing) stack->setCurrentIndex(1);

    const VideoRendererId vid = mediaPlayer->renderer()->id();
    if (vid == VideoRendererId_XV || vid == VideoRendererId_GLWidget2 || vid == VideoRendererId_OpenGLWidget)
        mediaPlayer->renderer()->forcePreferredPixelFormat(true);
    else mediaPlayer->renderer()->forcePreferredPixelFormat(false);

    if (video) qDebug("%s(%sDEBUG%s):%s Setando rendenização %s %i ...\033[0m", GRE, RED, GRE, BLU,
                      qUtf8Printable(op), video->id());
}


/** Alterando a dimensão de tela */
void OMPlayer::switchAspectRatio(QAction *action) {
    Utils::Aspect r = Utils::Aspect(action->data().toInt());
    if (action == aspectAction) {
        action->toggle();
        return;
    }
    if (r == Utils::AspectVideo) mediaPlayer->renderer()->setOutAspectRatioMode(VideoRenderer::VideoAspectRatio);
    else if (r == Utils::AspectAuto) mediaPlayer->renderer()->setOutAspectRatioMode(VideoRenderer::RendererAspectRatio);
    else {
        mediaPlayer->renderer()->setOutAspectRatioMode(VideoRenderer::CustomAspectRation);
        mediaPlayer->renderer()->setOutAspectRatio(Utils::aspectNum(r));
    }
    aspectAction->setChecked(false);
    aspectAction = action;
    aspectAction->setChecked(true);
}


/** Controle da velocidade do reprodutor */
void OMPlayer::onSpinBoxChanged(double val) {
    if (playing && !mediaPlayer->isPaused() && mediaPlayer->isPlaying()) mediaPlayer->setSpeed(val);
}


/** Função que possui o propósito de ajustar a ordem de execução ao remover itens da playlist anteriores ao
 * arquivo multimídia sendo reproduzido no momento */
void OMPlayer::ajustActualItem(int item) {
    listnum.clear();  /** É preferível redefinir a contagem aleatória */
    setTotalItems();
    if (!playing) return;

    if (item < actualitem) {
        playlist->selectCurrent(previousitem);
        actualitem = previousitem;
        nextitem = actualitem + 1;
        previousitem = actualitem - 1;
        if (actualitem == 0) previousitem = playlist->setListSize() - 1;
        if (actualitem == playlist->setListSize() - 1) nextitem = 0;
        return;
    }

    if (item == actualitem) {
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

    if (item > actualitem) playlist->selectCurrent(actualitem);
}


/** Calculando o total de itens */
void OMPlayer::setTotalItems() {
    totalitems = playlist->setListSize();
    qDebug("%s(%sDEBUG%s):%s Total de itens na playlist: %i ... \033[0m", GRE, RED, GRE, BLU, totalitems);
}


/** Setando o item atualmente selecionado */
void OMPlayer::setSelect(int item) {
    if (!playing) {
        actualitem = item;
        qDebug("%s(%sDEBUG%s):%s Selecionando o item %s manualmente ...\033[0m", GRE, RED, GRE, ORA,
               qUtf8Printable(Utils::mediaTitle(playlist->getItems(item))));
    }
}


/** Função geral para execução de arquivos multimídia */
void OMPlayer::play(const QString &isplay, int index) {
    this->setWindowTitle(Utils::mediaTitle(isplay));
    speedBox->setValue(1);
    mediaPlayer->setSpeed(1);
    mediaPlayer->play(isplay);
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
    }
}


/** Para executar os itens recém adicionados da playlist */
void OMPlayer::firstPlay(const QString &isplay, int pos) {
    if (!mediaPlayer->isPlaying()) {
        qDebug("%s(%sDEBUG%s):%s Reproduzindo um Arquivo Multimídia ...\033[0m", GRE, RED, GRE, ORA);
        play(isplay, pos);
    }
}


/** Executa um item selecionado da playlist */
void OMPlayer::doubleplay(const QString &name) {
    qDebug("%s(%sDEBUG%s):%s Reproduzindo um Arquivo Multimídia ...\033[0m", GRE, RED, GRE, ORA);
    play(name, playlist->selectItems());
}


/** Função para selecionar aleatóriamente a próxima mídia */
void OMPlayer::nextRand() {
    qDebug("%s(%sDEBUG%s):%s Reproduzindo uma mídia aleatória ...\033[0m", GRE, RED, GRE, ORA);
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

    qDebug("%s(%sDEBUG%s):%s Número de itens reproduzidos aleatoriamente: %i ...\033[0m",
           GRE, RED, GRE, BLU, listnum.size());
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

        qDebug("%s(%sDEBUG%s):%s Reproduzindo o próximo item ...\033[0m", GRE, RED, GRE, ORA);
        play(playlist->getItems(nextitem), nextitem);
    }
}


/** Botão previous */
void OMPlayer::Previous() {
    if (mediaPlayer->isPlaying() || playing) {
        qDebug("%s(%sDEBUG%s):%s Reproduzindo um item anterior ...\033[0m", GRE, RED, GRE, ORA);
        play(playlist->getItems(previousitem), previousitem);
    }
}


/** Função para o botão de play/pause */
void OMPlayer::playPause() {
    if (!mediaPlayer->isPlaying()) {
        if (playlist->setListSize() > 0) {
            qDebug("%s(%sDEBUG%s):%s Reproduzindo um Arquivo Multimídia ...\033[0m", GRE, RED, GRE, ORA);
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
}


/** Função para alterar o botão play/pause */
void OMPlayer::onPaused(bool paused) {
    if (paused) {
        qDebug("%s(%sDEBUG%s):%s Pausando ...\033[0m", GRE, RED, GRE, ORA);
        Utils::changeIcon(playBtn, "play", tr("Play"));
    } else {
        qDebug("%s(%sDEBUG%s):%s Reproduzindo ...\033[0m", GRE, RED, GRE, ORA);
        Utils::changeIcon(playBtn, "pause", tr("Pause"));
    }
}


/** Função que define alguns parâmetros ao iniciar a reprodução */
void OMPlayer::onStart() {
    playing = true;

    Utils::changeIcon(playBtn, "pause", tr("Pause"));
    stack->setCurrentIndex(1);
    slider->setDisabled(false);
    infoview->setStatistics(mediaPlayer->statistics());

    /** Definindo dimensões para o preview */
    Width = mediaPlayer->statistics().video_only.width;
    Height = mediaPlayer->statistics().video_only.height;

    /** Atualizando o status do item da playlist se necessário */
    if (playlist->setDuration() == 0) {
        int row = playlist->selectItems();
        QString url = mediaPlayer->file();
        qint64 duration = mediaPlayer->mediaStopPosition();

        MI.Open(url.toStdWString());
        QString format = QString::fromStdWString(MI.Get(MediaInfoDLL::Stream_General, 0, __T("Format"),
                                                        MediaInfoDLL::Info_Text, MediaInfoDLL::Info_Name));
        MI.Close();

        qDebug("%s(%sDEBUG%s):%s Atualizando %s ...\033[0m", GRE, RED, GRE, UPD, qUtf8Printable(url));
        playlist->removeSelectedItems(PlayList::Update);
        playlist->insert(url, row, duration, format, PlayList::Update);
    }

    /** Definindo o tempo de duração no slider */
    slider->setMinimum(int(mediaPlayer->mediaStartPosition()));
    slider->setMaximum(int(mediaPlayer->mediaStopPosition() - Utils::setDifere(unit))/ unit);
    end->setText(QTime(0, 0, 0).addMSecs(
            int(mediaPlayer->mediaStopPosition())).toString(QString::fromLatin1("HH:mm:ss")));

    onTimeSliderLeave(IsPlay);
    changeIcons(IsPlay);
    updateChannelMenu();
    updateSlider(mediaPlayer->position());
    screensaver->disable();
}


/** Função que define alguns parâmetros ao parar a reprodução */
void OMPlayer::onStop() {
    if (!playing) {
        qDebug("%s(%sDEBUG%s):%s Finalizando a Reprodução ...\033[0m", GRE, RED, GRE, ORA);
        Utils::changeIcon(playBtn, "play", tr("Play"));
        this->setWindowTitle(QString(PRG_NAME));

        stack->setCurrentIndex(0);
        slider->setMaximum(0);
        slider->setDisabled(true);
        current->setText("-- -- : -- --");
        end->setText("-- -- : -- --");

        onTimeSliderLeave(IsPlay);
        changeIcons(IsPlay);
        updateChannelMenu();
        screensaver->enable();
    }
}


/** Ativação do modo de repetição da mídia */
void OMPlayer::setRepeat() {
    if (mediaPlayer->repeat() == 0) mediaPlayer->setRepeat(-1);
    else mediaPlayer->setRepeat(0);
}


/** Ativação do modo de repetição da playlist */
void OMPlayer::setReplay() {
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


/** Função para ativar a função pause com um clique */
void OMPlayer::enablePause() { if (!enterpos) pausing = true; }


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
    qDebug("%s(%sDEBUG%s):%s Entrando no Modo Tela Cheia ...\033[0m", GRE, RED, GRE, ORA);

    if (this->isMaximized()) maximize = true; /** Mapear interface maximizada */
    this->showFullScreen();
    fadeWctl(Hide);
    filter->setMove(false);
    enterpos = false;
}


/** Sair do modo tela cheia */
void OMPlayer::leaveFullScreen() {
    qDebug("%s(%sDEBUG%s):%s Saindo do Modo Tela Cheia ...\033[0m", GRE, RED, GRE, ORA);
    this->showNormal();

    /** Precisa caso a interface já estava maximizada antes. Note que showMaximized() só funciona após a execução
     * de showNormal(), a tela não pula direto de fulscreen para maximizado. */
    if (maximize) this->showMaximized();
    fadeWctl(Hide);
    filter->setMove(false);
    enterpos = maximize = false;
}


/** Efeito fade bacana para os controles e a playlist */
void OMPlayer::fadeWctl(OMPlayer::FADE option) {
    auto *animation = new QPropertyAnimation(effect, "opacity");
    auto *animation2 = new QPropertyAnimation(effect2, "opacity");
    animation->setDuration(120);
    animation2->setDuration(120);

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
            if (!contmenu) blankMouse();
            contmenu = false;
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
    showsett = true;
    filter->setSett(showsett);
    this->setMaximumSize(size);
    this->setMinimumSize(size);

    if (dialog == AboutD) {
        qDebug("%s(%sDEBUG%s):%s Iniciando o diálogo sobre ...\033[0m", GRE, RED, GRE, CYA);
        about->show();
    } else if (dialog == SettingsD) {
        qDebug("%s(%sDEBUG%s):%s Iniciando o diálogo de configurações ...\033[0m", GRE, RED, GRE, CYA);
        sett->show();
    } else if (dialog == InfoD) {
        qDebug("%s(%sDEBUG%s):%s Iniciando o diálogo de informações de mídia ...\033[0m", GRE, RED, GRE, CYA);
        playlist->hideFade();
        setHide();
        infoview->show();
    }
}


/** Função para executar operações ao fachar os diálogos */
void OMPlayer::closeDialog() {
    this->setMaximumSize(screen);
    this->setMinimumSize(min);
    showsett = false;
    filter->setSett(showsett);
}


/** Ativar ocultação  */
void OMPlayer::hideTrue() {
    if (enterpos) {
        qDebug("%s(%sDEBUG%s):%s Ocultação liberada ...\033[0m", GRE, RED, GRE, SHW);
        enterpos = false;
    }
    filter->setFixed(enterpos);
}


/** Desativar ocultação */
void OMPlayer::hideFalse() {
    if (!enterpos) {
        qDebug("%s(%sDEBUG%s):%s Ocultação impedida ...\033[0m", GRE, RED, GRE, HID);
        enterpos = true;
    }
    filter->setFixed(enterpos);
}


/** Mouse sobre os itens da playlist */
void OMPlayer::enterList() {
    hideFalse();
    listmenu = true;
}


/** Mouse fora dos itens da playlist */
void OMPlayer::leaveList() {
    listmenu = false;
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
        qDebug("%s(%sDEBUG%s):%s Exibindo a pré-visualização ...\033[0m", GRE, RED, GRE, CYA);
    ispreview = true;
    hideFalse();
}


/** Saindo da pré-visualização */
void OMPlayer::onTimeSliderLeave(OMPlayer::STATUS status) {
    if (preview->isVisible()) {
        qDebug("%s(%sDEBUG%s):%s Finalizando a pré-visualização ...\033[0m", GRE, RED, GRE, CYA);
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
    qDebug("%s(%sDEBUG%s):%s Atualizando posição de execução %lld / %lld ...\033[0m", GRE, RED, GRE, ORA,
           pos, mediaPlayer->position());
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
    /** O volume do reprodutor é definido com um valor do tipo double que vai de 0.0 a 1.0, portanto o valor
     * do tipo int a ser recebido, precisa ser reajustado e convertido. */
    mediaPlayer->audio()->setVolume((double)value / 100);
}


/** Ação ao terminar a atualização do volume */
void OMPlayer::volumeFinished(qreal pos) {
    int value = int(pos * 100);
    qDebug("%s(%sDEBUG%s):%s Alterando o volume para %i ...\033[0m", GRE, RED, GRE, ORA, value);
    QString tooltip = tr("Volume") + ": " + QString::number(value);
    muted = false;

    if (JsonTools::floatJson("volume") != pos) JsonTools::floatJson("volume", (float)pos);

    if (playing && !mediaPlayer->statistics().audio.available) {
        Utils::changeIcon(volumeBtn, "nosound", tr("No Video Sound"));
        return;
    }

    if (value == 0) {
        Utils::changeIcon(volumeBtn, "mute", tr("Muted"));
        muted = true;
    } else if (value > 0 && value <= 25) Utils::changeIcon(volumeBtn, "volume_low", tooltip);
    else if (value > 25 && value <= 75) Utils::changeIcon(volumeBtn, "volume_medium", tooltip);
    else if (value > 75) Utils::changeIcon(volumeBtn, "volume_high", tooltip);
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

    AudioOutput *ao = mediaPlayer ? mediaPlayer->audio() : nullptr;
    if (!ao) return;

    AudioFormat::ChannelLayout cl = ao->audioFormat().channelLayout();
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
    AudioFormat::ChannelLayout cl = (AudioFormat::ChannelLayout)action->data().toInt();
    AudioOutput *ao = mediaPlayer ? mediaPlayer->audio() : nullptr;
    if (!ao) return;

    channelAction->setChecked(false);
    channelAction = action;
    channelAction->setChecked(true);

    if (!ao->close()) {
        qDebug("%s(%sDEBUG%s)%s::%s Falha ao fechar o áudio ...", GRE, RED, GRE, RED, ERR);
        return;
    }

    /** Alterando o formato */
    AudioFormat af(ao->audioFormat());
    af.setChannelLayout(cl);
    ao->setAudioFormat(af);

    if (!ao->open()) qDebug("%s(%sDEBUG%s)%s::%s Falha ao abrir o áudio ...", GRE, RED, GRE, RED, ERR);
}


/** Função para atualizar a barra de progresso de execução */
void OMPlayer::updateSlider(qint64 value) {
    if (mediaPlayer->isSeekable()) slider->setValue(int(value / unit));
    current->setText(QTime(0, 0, 0).addMSecs(int(value)).toString(QString::fromLatin1("HH:mm:ss")));
    infoview->setCurrentTime(int(value));

    /** Próxima mídia */
    if (int(value) > mediaPlayer->duration() - Utils::setDifere(unit)) {
        if (mediaPlayer->repeat() != 0) return;
        if (actualitem == playlist->setListSize() - 1 && !restart && !randplay) {
            playing = false;
            mediaPlayer->stop();
            playlist->selectClean();
            previousitem = playlist->setListSize() - 1;
            actualitem = 0;
            nextitem = 1;
        } else Next();
    }
}


/** Função para atualizar a barra de progresso de execução */
void OMPlayer::updateSliderUnit() {
    unit = mediaPlayer->notifyInterval();
    slider->setMaximum(int(mediaPlayer->mediaStopPosition() - Utils::setDifere(unit))/ unit);
    updateSlider(mediaPlayer->position());
}


/** Função para recarregar os ícones do programa */
void OMPlayer::changeIcons(OMPlayer::STATUS change) {
    int value = int(mediaPlayer->audio()->volume() * 100);
    QString tooltip = tr("Volume") + ": " + QString::number(value);

    if (playing && !mediaPlayer->statistics().audio.available)
        Utils::changeIcon(volumeBtn, "nosound", tr("No Video Sound"));
    else if (value > 0 && value <= 25) Utils::changeIcon(volumeBtn, "volume_low", tooltip);
    else if (value > 25 && value <= 75) Utils::changeIcon(volumeBtn, "volume_medium", tooltip);
    else if (value > 75) Utils::changeIcon(volumeBtn, "volume_high", tooltip);
    else if (value == 0) Utils::changeIcon(volumeBtn, "mute", tr("Muted"));

    if (change != IsPlay) {
        if (mediaPlayer->isPlaying()) Utils::changeIcon(playBtn, "pause", tr("Pause"));
        else Utils::changeIcon(playBtn, "play", tr("Play"));

        if (restart) Utils::changeIcon(replayBtn, "replay-on");
        else Utils::changeIcon(replayBtn, "replay");
        if (randplay) Utils::changeIcon(shuffleBtn, "shuffle-on");
        else Utils::changeIcon(shuffleBtn, "shuffle");

        Utils::changeIcon(stopBtn, "stop");
        Utils::changeIcon(nextBtn, "next");
        Utils::changeIcon(previousBtn, "previous");
        playlist->changeIcons();
        about->changeIcons();
    }
}


/** Função para mapear o status de um arquivo multimídia */
void OMPlayer::onMediaStatusChanged() {
    QString status;
    auto *player = reinterpret_cast<AVPlayer*>(sender());
    if (!player) return;

    switch (player->mediaStatus()) {
        case InvalidMedia:
            invalid = true;
            break;
        case BufferedMedia:
            status = "Buffered !";
            break;
        case LoadedMedia:
            status = "Loaded !";
            break;
        default:
            qDebug("%s(%sDEBUG%s):%s Status não mapeado:%s %i\033[0m", GRE, RED, GRE, RDL, ERR, player->mediaStatus());
            break;
    }

    if (!status.isEmpty())
        qDebug("%s(%sDEBUG%s):%s Status do Arquivo Multimídia %s\033[0m", GRE, RED, GRE, SND, qUtf8Printable(status));
}


/** Debug em caso de eventuais erros */
void OMPlayer::handleError(const AVError &error) {
    string tr = error.string().toStdString();
    Utils::rm_nl(tr);
    qDebug("%s(%sAVError%s)%s::%s %s", GRE, RED, GRE, RED, ERR, tr.c_str());

    if (!QFileInfo::exists(mediaPlayer->file()) || invalid) {
        if (actualitem == playlist->setListSize() - 1 && !restart && !randplay) {
            playing = false;
            onStop();
            playlist->selectClean();
            previousitem = playlist->setListSize() - 1;
            actualitem = 0;
            nextitem = 1;
        } else
            Next();
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
    wctl->setFixedSize(event->size());
    QWidget::resizeEvent(event);
}


/** Mapeamento dos eventos de maximizar, minimizar e restaurar */
void OMPlayer::changeEvent(QEvent *event) {
    if(event->type() == QEvent::WindowStateChange) {
        if (this->windowState() == Qt::WindowNoState) {
            qDebug("%s(%sDEBUG%s):%s Restaurando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, ORA);
            JsonTools::boolJson("maximized", false);
        }
        else if (this->windowState() == Qt::WindowMinimized) {
            qDebug("%s(%sDEBUG%s):%s Minimizando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, ORA);
            if (showsett) this->showNormal();
        }
        else if (this->windowState() == Qt::WindowMaximized) {
            qDebug("%s(%sDEBUG%s):%s Maximizando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, ORA);
            JsonTools::boolJson("maximized", true);
            playlist->hideFade();

        }
        else if (this->windowState() == (Qt::WindowMinimized|Qt::WindowMaximized)) {
            qDebug("%s(%sDEBUG%s):%s Minimizando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, ORA);
            if (showsett) this->showNormal();
        }
    }
}


/** Ação ao fechar o programa */
void OMPlayer::closeEvent(QCloseEvent *event) {
    screensaver->enable();
    playlist->save();
    qDebug("%s(%sDEBUG%s):%s Finalizando o Reprodutor Multimídia !\033[0m", GRE, RED, GRE, CYA);
    event->accept();
}


/**********************************************************************************************************************/


/** Função para o menu de contexto do programa */
void OMPlayer::ShowContextMenu(const QPoint &pos) {
    if (listmenu || !enterpos) {
        qDebug("%s(%sDEBUG%s):%s Iniciando o Menu de Contexto ...\033[0m", GRE, RED, GRE, CYA);
        arrowMouse();
    }

    contmenu = true;
    auto *contextMenu = new CMenu(nullptr, this);

    if (listmenu && enterpos) {
        control = true;
        contextMenu->setWindowOpacity(0.9);

        /** Menu de informação de mídia */
        QAction mediainfo(TR("Current Media Info"), this);
        Utils::changeMenuIcon(mediainfo, "about");
        connect(&mediainfo, &QAction::triggered, [this](){ setDialog(InfoD); });

        QAction saveplaylist(TR("Save Playlist"), this);

//        contextMenu->addAction(&saveplaylist);
        contextMenu->addAction(&mediainfo);
        contextMenu->exec(mapToGlobal(pos));
        return;
    }

    if (!enterpos) {
        /** Opções de menu*/
        auto *other = new CMenu(TR("Other Options"), this);
//        auto *visualization = new CMenu(TR("Visualizations"), this);
        auto *optionVideo = new CMenu(TR("Video Options"), this);

        /** Modo repetição */
        QAction repeat(TR("Repeat Mode"), this);
        repeat.setShortcut(QKeySequence(CTRL | ALT | Qt::Key_T));
        if (mediaPlayer->repeat() == 0) Utils::changeMenuIcon(repeat, "repeat");
        else Utils::changeMenuIcon(repeat, "repeat-on");
        connect(&repeat, SIGNAL(triggered()), SLOT(setRepeat()));

        /** Dimensão de exibição */
        QAction rotation(TR("Rotation"), this);

        /** Menu de abrir */
        QAction open(TR("Open Files"), this);
        open.setShortcut(QKeySequence(CTRL | Qt::Key_O));
        Utils::changeMenuIcon(open, "folder");
        connect(&open, SIGNAL(triggered()), SLOT(openMedia()));

        /** Menu tela cheia */
        QAction fullscreen(TR("Show Fullscreen"), this);
        if (this->isFullScreen())
            fullscreen.setText(TR("Exit Fullscreen"));
        fullscreen.setShortcut(QKeySequence(ALT | Qt::Key_Enter));
        Utils::changeMenuIcon(fullscreen, "fullscreen");
        connect(&fullscreen, SIGNAL(triggered()), SLOT(changeFullScreen()));

        /** Menu aleatório */
        QAction shuffle(TR("Shuffle"), this);
        shuffle.setShortcut(QKeySequence(CTRL | Qt::Key_H));
        Utils::changeMenuIcon(shuffle, "shuffle-menu");
        connect(&shuffle, SIGNAL(triggered()), SLOT(setShuffle()));

        /** Menu replay */
        QAction replay(TR("Replay"), this);
        replay.setShortcut(QKeySequence(CTRL | Qt::Key_T));
        Utils::changeMenuIcon(replay, "replay-menu");
        connect(&replay, SIGNAL(triggered()), SLOT(setReplay()));

        /** Menu de configuração */
        QAction settings(TR("Settings"), this);
        settings.setShortcut(QKeySequence(ALT | Qt::Key_S));
        Utils::changeMenuIcon(settings, "settings");
        connect(&settings, &QAction::triggered, [this](){ setDialog(SettingsD); });

        /** Menu sobre */
        QAction mabout(TR("About"), this);
        Utils::changeMenuIcon(mabout, "about");
        connect(&mabout, &QAction::triggered, [this](){ setDialog(AboutD); });

        /** Montagem do menu de opções de vídeo */
        optionVideo->addMenu(aspectratio);
//        optionVideo->addAction(&rotation);

        /** Montagem do menu para outras opções */
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
