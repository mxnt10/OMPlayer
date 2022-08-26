#include <QGraphicsOpacityEffect>
#include <QMenu>
#include <QRandomGenerator>
#include <QToolTip>

#include <MediaInfoDLL.h>
#include <ScreenSaver>
#include <filesystem>

#include "ClickableMenu.h"
#include "JsonTools.h"
#include "Player.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Construtor que define a interface do programa */
OMPlayer::OMPlayer(QWidget *parent) : QWidget(parent) {
    qDebug("%s(%sDEBUG%s):%s Iniciando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, CYA);
    this->setWindowTitle(QString(PRG_NAME));
    this->setWindowIcon(QIcon(Utils::setIcon()));
    this->setMinimumSize(min);
    this->setMouseTracking(true);
    this->setAutoFillBackground(true);
    this->move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center());
    this->setStyleSheet(Utils::setStyle("global"));
    ScreenSaver::instance().disable();
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


    /** Janelas de configurações do programa, informações e demais */
    about = new About(this);
    sett = new Settings(this);
    infoview = new StatisticsView(this);
    connect(sett, SIGNAL(emitvalue(QString)), this, SLOT(setRenderer(QString)));
    connect(sett, SIGNAL(changethemeicon()), this, SLOT(changeIcons()));
    connect(sett, SIGNAL(emitclose()), this, SLOT(closeDialog()));
    connect(infoview, SIGNAL(emitclose()), this, SLOT(closeDialog()));
    connect(about, SIGNAL(emitclose()), this, SLOT(closeDialog()));


    /** Parte principal do programa que permite o funcionamento do reprodutor */
    mediaPlayer = new AVPlayer(this);
    setRenderer(JsonTools::stringJson("renderer"));
    connect(mediaPlayer->audio(), SIGNAL(volumeChanged(qreal)), SLOT(volumeFinished(qreal)));
    connect(mediaPlayer, SIGNAL(seekFinished(qint64)), SLOT(seekFinished(qint64)));
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
    connect(mediaPlayer, SIGNAL(notifyIntervalChanged()), SLOT(updateSliderUnit()));
    connect(mediaPlayer, SIGNAL(started()), SLOT(onStart()));
    connect(mediaPlayer, SIGNAL(stopped()), SLOT(onStop()));
    connect(mediaPlayer, SIGNAL(paused(bool)), SLOT(onPaused(bool)));
    connect(mediaPlayer, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)), SLOT(onMediaStatusChanged()));
    connect(mediaPlayer, SIGNAL(error(QtAV::AVError)), SLOT(handleError(QtAV::AVError)));


    /** Playlist do reprodutor */
    playlist = new PlayList();
    setTotalItems();
    connect(playlist, SIGNAL(aboutToPlay(QString)), SLOT(doubleplay(QString)));
    connect(playlist, SIGNAL(firstPlay(QString,int)), SLOT(firstPlay(QString,int)));
    connect(playlist, SIGNAL(selected(int)), SLOT(setSelect(int)));
    connect(playlist, SIGNAL(emitremove(int)), SLOT(ajustActualItem(int)));
    connect(playlist, SIGNAL(emithiden()), SLOT(setHide()));
    connect(playlist, SIGNAL(emithide()), SLOT(hideTrue()));
    connect(playlist, SIGNAL(emitnohide()), SLOT(hideFalse()));
    connect(playlist, SIGNAL(enterListView()), SLOT(enterList()));
    connect(playlist, SIGNAL(leaveListView()), SLOT(leaveList()));
    connect(playlist, SIGNAL(emitstop()), SLOT(setStop()));
    connect(playlist, SIGNAL(emitItems()), SLOT(setTotalItems()));


    /** Barra de progresso de execução */
    slider = new Slider(this, true, (-1), 28, 0);
    connect(slider, SIGNAL(onHover(int,int)), SLOT(onTimeSliderHover(int,int)));
    connect(slider, SIGNAL(sliderMoved(int)), SLOT(seekBySlider(int)));
    connect(slider, SIGNAL(emitEnter()), SLOT(onTimeSliderEnter()));
    connect(slider, SIGNAL(emitLeave()), SLOT(onTimeSliderLeave()));
    connect(slider, SIGNAL(sliderPressed()), SLOT(seekBySlider()));


    /** Labels para mostrar o tempo de execução e a duração */
    current = new Label(CENTER, 70, "Current time", "-- -- : -- --");
    connect(current, SIGNAL(emitEnter()), SLOT(hideFalse()));
    connect(current, SIGNAL(emitLeave()), SLOT(hideTrue()));
    end = new Label(CENTER, 70, "Duration", "-- -- : -- --");
    connect(end, SIGNAL(emitEnter()), SLOT(hideFalse()));
    connect(end, SIGNAL(emitLeave()), SLOT(hideTrue()));


    /** Botões para os controles de reprodução */
    playBtn = new Button(Button::button, "play", 48, tr("Play"));
    connect(playBtn, SIGNAL(clicked()), SLOT(playPause()));
    connect(playBtn, SIGNAL(emitEnter()), SLOT(hideFalse()));
    stopBtn = new Button(Button::button, "stop", 32, tr("Stop"));
    connect(stopBtn, SIGNAL(clicked()), SLOT(setStop()));
    connect(stopBtn, SIGNAL(emitEnter()), SLOT(hideFalse()));
    nextBtn = new Button(Button::button, "next", 32, tr("Next"));
    connect(nextBtn, SIGNAL(clicked()), SLOT(Next()));
    connect(nextBtn, SIGNAL(emitEnter()), SLOT(hideFalse()));
    previousBtn = new Button(Button::button, "previous", 32, tr("Previous"));
    connect(previousBtn, SIGNAL(clicked()), SLOT(Previous()));
    connect(previousBtn, SIGNAL(emitEnter()), SLOT(hideFalse()));
    replayBtn = new Button(Button::button, "replay", 32, tr("Replay"));
    connect(replayBtn, SIGNAL(clicked()), SLOT(setReplay()));
    connect(replayBtn, SIGNAL(emitEnter()), SLOT(hideFalse()));
    shuffleBtn = new Button(Button::button, "shuffle", 32, tr("Shuffle"));
    connect(shuffleBtn, SIGNAL(clicked()), SLOT(setShuffle()));
    connect(shuffleBtn, SIGNAL(emitEnter()), SLOT(hideFalse()));
    volumeBtn = new Button(Button::button, "volume_high", 32, tr("Volume") + ": " + QString::number(vol * 100));
    connect(volumeBtn, SIGNAL(clicked()), SLOT(setMute()));
    connect(volumeBtn, SIGNAL(emitEnter()), SLOT(hideFalse()));

    if (JsonTools::boolJson("on_replay")) setReplay();
    if (JsonTools::boolJson("on_shuffle")) setShuffle();


    /** Controle do volume */
    volume = new Slider(this, false, 90, (-1), 100, "volume");
    volume->setValue(int(vol * 100));
    mediaPlayer->audio()->setVolume(vol);
    connect(volume, SIGNAL(onHover(int,int)), SLOT(onTimeVolume(int,int)));
    connect(volume, SIGNAL(sliderMoved(int)), SLOT(setVolume(int)));
    connect(volume, SIGNAL(sliderPressed()), SLOT(setVolume()));
    connect(volume, SIGNAL(emitEnter()), SLOT(hideFalse()));


    /** Assistentes para mapear quando a ocultação dos controles não deve ser feita */
    qDebug("%s(%sDEBUG%s):%s Preparando o layout da interface ...\033[0m", GRE, RED, GRE, CYA);
    auto *enterfilter = new EventFilter(this, EventFilter::Other);
    auto *nohideleft = new QWidget();
    auto *nohideright = new QWidget();
    nohideleft->installEventFilter(enterfilter);
    nohideright->installEventFilter(enterfilter);
    connect(enterfilter, SIGNAL(emitEnter()), SLOT(hideFalse()));


    /** Plano de fundo semitransparente dos controles de reprodução */
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
    buttons->addWidget(nohideleft);
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
    buttons->addWidget(nohideright);


    /** Ajustes na barra de execução para inserir o tempo de execução e a duração */
    auto *fgslider = new QHBoxLayout();
    fgslider->addWidget(current);
    fgslider->addWidget(slider);
    fgslider->addWidget(end);


    /** Ajuste dos controles */
    auto *fgctl = new QVBoxLayout();
    fgctl->setContentsMargins(10, 12, 10, 22);
    fgctl->addLayout(fgslider);
    fgctl->addLayout(buttons);


    /** Widget separado do pai para os controles e a playlist */
    wctl = new QWidget(this);
    wctl->setMouseTracking(true);
    wctl->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    wctl->setAttribute(Qt::WA_NoSystemBackground, true);
    wctl->setAttribute(Qt::WA_TranslucentBackground, true);


    /** Layout dos controles de reprodução */
    auto *controls = new QGridLayout(wctl);
    controls->setMargin(0);
    controls->setSpacing(0);
    controls->addWidget(playlist, 0, 0);
    controls->addLayout(bgctl, 1, 0);
    controls->addLayout(fgctl, 1, 0);
    controls->setAlignment(BOTTON);


    /** Definição da logo */
    logo = new Label(CENTER);
    logo->setPixmap(QPixmap(Utils::setIcon(Utils::Logo)));


    /** Layout principal criado usando sobreposição de widgets */
    layout = new QGridLayout();
    layout->setMargin(0);
    layout->addWidget(video->widget(), 0, 0);
    this->setLayout(layout);


    /** Filtro de eventos */
    filter = new EventFilter(this, EventFilter::General);
    auto *cfilter = new EventFilter(this, EventFilter::Control);
    this->installEventFilter(filter);
    wctl->installEventFilter(filter);
    wctl->installEventFilter(cfilter);
    connect(filter, SIGNAL(emitMouseMove()), SLOT(setShow()));
    connect(filter, SIGNAL(emitDoubleClick()), SLOT(changeFullScreen()));
    connect(filter, SIGNAL(emitMousePress()), SLOT(enablePause()));
    connect(filter, SIGNAL(emitMouseRelease()), SLOT(clickCount()));
    connect(filter, SIGNAL(customContextMenuRequested(QPoint)), SLOT(ShowContextMenu(QPoint)));
    connect(filter, SIGNAL(emitOpen()), SLOT(openMedia()));
    connect(filter, SIGNAL(emitEsc()), SLOT(leaveFullScreen()));
    connect(filter, SIGNAL(emitShuffle()), SLOT(setShuffle()));
    connect(filter, SIGNAL(emitReplay()), SLOT(setReplay()));
    connect(filter, SIGNAL(emitReplayOne()), SLOT(setRepeat()));
    connect(filter, SIGNAL(emitSettings()), SLOT(setSettings()));
    connect(filter, SIGNAL(emitPlay()), SLOT(playPause()));
    connect(filter, SIGNAL(emitNext()), SLOT(Next()));
    connect(filter, SIGNAL(emitPrevious()), SLOT(Previous()));
    connect(filter, SIGNAL(emitLeave()), SLOT(setHide()));
    connect(cfilter, SIGNAL(emitLeave()), SLOT(setHide()));


    /** Temporizador para o cálculo do número de cliques em 300ms */
    click = new QTimer(this);
    click->setSingleShot(true);
    click->setInterval(300);
    connect(click, SIGNAL(timeout()), this, SLOT(detectClick()));


    /** Menu dos canais de áudio para o menu de contexto */
    auto *effect = new QGraphicsOpacityEffect();
    effect->setOpacity(0.8);
    channel = new QMenu(this);
    auto *subMenu = new ClickableMenu(tr("Channel"));
    subMenu->setGraphicsEffect(effect);
    subMenu->setStyleSheet(Utils::setStyle("action"));
    channel = subMenu;
    connect(subMenu, SIGNAL(triggered(QAction*)), SLOT(changeChannel(QAction*)));
    subMenu->addAction(tr("As input"))->setData(AudioFormat::ChannelLayout_Unsupported);
    subMenu->addAction(tr("Stereo"))->setData(AudioFormat::ChannelLayout_Stereo);
    subMenu->addAction(tr("Mono (Center)"))->setData(AudioFormat::ChannelLayout_Center);
    subMenu->addAction(tr("Left"))->setData(AudioFormat::ChannelLayout_Left);
    subMenu->addAction(tr("Right"))->setData(AudioFormat::ChannelLayout_Right);
    auto *ag = new QActionGroup(subMenu);
    ag->setExclusive(true);


    /** Verificando o que foi clicado */
    foreach(QAction* action, subMenu->actions()) {
        ag->addAction(action);
        action->setCheckable(true);
    }
}


/** Destrutor */
OMPlayer::~OMPlayer() = default;


/**********************************************************************************************************************/


/** Função para abrir arquivos multimídia */
void OMPlayer::openMedia(const QStringList &parms) {
    playlist->addItems(parms);
}


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
    VideoRenderer *vo{};

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
        if (QString::compare(op, rend[i].name, Qt::CaseSensitive) == 0)
            vo = VideoRenderer::create(rend[i].id);
    }

    /** Verificando se o layout já foi criado. É necessário readicionar o widget ao mudar a renderização. */
    if (layout) layout->removeWidget(video->widget());

    if (vo && vo->isAvailable())
        video = new VideoOutput(vo->id(), this);
    else video = new VideoOutput(this);
    video->widget()->setMouseTracking(true);
    mediaPlayer->setRenderer(video);

    /** Readicionando o widget ao layout */
    if (layout) layout->addWidget(video->widget(), 0, 0);

    const VideoRendererId vid = mediaPlayer->renderer()->id();
    if (vid == VideoRendererId_XV || vid == VideoRendererId_GLWidget2 || vid == VideoRendererId_OpenGLWidget)
        mediaPlayer->renderer()->forcePreferredPixelFormat(true);
    else mediaPlayer->renderer()->forcePreferredPixelFormat(false);

    if (vo) qDebug("%s(%sDEBUG%s):%s Setando rendenização %s %i ...\033[0m", GRE, RED, GRE, BLU,
                   qUtf8Printable(op), vo->id());
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
        if (actualitem == playlist->setListSize() - 1 && !restart && !randplay) {
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
    Utils::changeIcon(playBtn, "pause", tr("Pause"));
    slider->setDisabled(false);
    infoview->setStatistics(mediaPlayer->statistics());
    playing = true;

    /** Definindo dimensões para o preview */
    Width = mediaPlayer->statistics().video_only.width;
    Height = mediaPlayer->statistics().video_only.height;

    /** Atualizando o status do item da playlist se necessário */
    if (playlist->setDuration() == 0) {
        int row = playlist->selectItems();
        QString url = mediaPlayer->file();
        qint64 duration = mediaPlayer->mediaStopPosition();

        MI.Open(url.toStdString());
        QString format = MI.Get(Stream_General, 0, "Format", Info_Text, Info_Name).c_str();
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

    /** Desativando Bloqueio de tela */
    if (!isblock) {
        ScreenSaver::instance().active();
        isblock = true;
    }

    onTimeSliderLeave(IsPlay);
    changeIcons(IsPlay);
    updateChannelMenu();
    updateSlider(mediaPlayer->position());
}


/** Função que define alguns parâmetros ao parar a reprodução */
void OMPlayer::onStop() {
    if (!playing) {
        qDebug("%s(%sDEBUG%s):%s Finalizando a Reprodução ...\033[0m", GRE, RED, GRE, ORA);
        Utils::changeIcon(playBtn, "play", tr("Play"));
        this->setWindowTitle(QString(PRG_NAME));

        slider->setMaximum(0);
        slider->setDisabled(true);
        current->setText("-- -- : -- --");
        end->setText("-- -- : -- --");

        onTimeSliderLeave(IsPlay);
        changeIcons(IsPlay);
        updateChannelMenu();
        onTimeSliderLeave();
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
void OMPlayer::enablePause() {
    if (!enterpos) pausing = true;
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
    qDebug("%s(%sDEBUG%s):%s Entrando no Modo Tela Cheia ...\033[0m", GRE, RED, GRE, ORA);

    if (this->isMaximized()) maximize = true; /** Mapear interface maximizada */
    this->showFullScreen();
    wctl->close();
    Utils::blankMouse();
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

    wctl->close();
    Utils::blankMouse();
    filter->setMove(false);
    enterpos = maximize = false;
}


/** Função auxiliar para fechar os controles e a playlist */
void OMPlayer::setHide() {
    /** Prevenindo a ocultação dos controles ao abrir o menu na playlist */
    if (control) {
        control = false;
        return;
    }
    wctl->close();
    hideTrue();
    filter->setMove(false);
}


/** Função auxiliar para abrir os controles e a playlist */
void OMPlayer::setShow() {
    if (showsett) return;
    wctl->show();
    wctl->activateWindow();
}


/** Função para abrir as configurações */
void OMPlayer::setSettings() {
    qDebug("%s(%sDEBUG%s):%s Iniciando o diálogo de configurações ...\033[0m", GRE, RED, GRE, CYA);
    showsett = true;
    filter->setSett(showsett);
    this->setMaximumSize(size);
    this->setMinimumSize(size);
    sett->show();
}


/** Função para exibir o sobre */
void OMPlayer::setAbout() {
    qDebug("%s(%sDEBUG%s):%s Iniciando o diálogo sobre ...\033[0m", GRE, RED, GRE, CYA);
    showsett = true;
    filter->setSett(showsett);
    this->setMaximumSize(size);
    this->setMinimumSize(size);
    about->show();
}


/** Função para exibir a janela de informações de mídia atual */
void OMPlayer::showInfo() {
    showsett = true;
    filter->setSett(showsett);
    setHide();
    this->setMaximumSize(size);
    this->setMinimumSize(size);
    infoview->show();
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
    QPoint gpos = slider->pos();

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
void OMPlayer::onTimeSliderLeave(OMPlayer::ST status) {
    if (preview->isVisible()) {
        qDebug("%s(%sDEBUG%s):%s Finalizando a pré-visualização ...\033[0m", GRE, RED, GRE, CYA);
        preview->close();
        prev->close();
    }
    if (status == Default) {
        ispreview = false;
        hideTrue();
    }
}


/** Altera o tempo de execução ao pressionar ou mover a barra de progresso de execução */
void OMPlayer::seekBySlider(int value) {
    if (mediaPlayer->isPaused() && mediaPlayer->currentVideoStream() > (-1) &&
        mediaPlayer->statistics().video.frame_rate == 0) return;
    mediaPlayer->setSeekType(QtAV::AccurateSeek);
    mediaPlayer->seek(qint64(value) * unit);
}


/** Altera o tempo de execução ao pressionar a barra de progresso de execução */
void OMPlayer::seekBySlider() {
    seekBySlider(slider->value());
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
        setVolume();
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


/** Altera o volume do reprodutor ao pressionar a barra de volume */
void OMPlayer::setVolume() {
    setVolume(volume->value());
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
    if (value < 10 ) str = "0";

    /** Definição da posição na tela e exibição do tooltip */
    if (value > 0) {
        QPoint gpos = mapToGlobal(volume->pos() + QPoint(pos + 12, 5));
        QToolTip::showText(gpos, QString::asprintf(" %s%i ", str.c_str(), value), this);
    } else QToolTip::hideText();
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
void OMPlayer::changeIcons(OMPlayer::ST change) {
    QToolTip::hideText();
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

    if (!std::filesystem::exists(mediaPlayer->file().toStdString()) || invalid) {
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
        }
        else if (this->windowState() == (Qt::WindowMinimized|Qt::WindowMaximized)) {
            qDebug("%s(%sDEBUG%s):%s Minimizando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, ORA);
            if (showsett) this->showNormal();
        }
    }
}


/** Ação ao fechar o programa */
void OMPlayer::closeEvent(QCloseEvent *event) {
    ScreenSaver::instance().enable();
    playlist->save();
    qDebug("%s(%sDEBUG%s):%s Finalizando o Reprodutor Multimídia !\033[0m", GRE, RED, GRE, CYA);
    event->accept();
}


/**********************************************************************************************************************/


/** Função para o menu de contexto do programa */
void OMPlayer::ShowContextMenu(const QPoint &pos) {
    if (listmenu || !enterpos) {
        qDebug("%s(%sDEBUG%s):%s Iniciando o Menu de Contexto ...\033[0m", GRE, RED, GRE, CYA);
        Utils::arrowMouse();
    }

    auto *contextMenu = new QMenu(this);
    contextMenu->setWindowOpacity(0.8);
    contextMenu->setStyleSheet(Utils::setStyle("contextmenu"));

    if (listmenu && enterpos) {
        control = true;
        contextMenu->setWindowOpacity(0.9);

        /** Menu de informação de mídia */
        QAction mediainfo(tr("Current Media Info"), this);
        Utils::changeMenuIcon(mediainfo, "about");
        connect(&mediainfo, SIGNAL(triggered()), SLOT(showInfo()));

        QAction saveplaylist(tr("Save Playlist"), this);

//        contextMenu->addAction(&saveplaylist);
        contextMenu->addAction(&mediainfo);
        contextMenu->exec(mapToGlobal(pos));
        return;
    }

    if (!enterpos) {
        /** Outras opções */
        auto *other = new QMenu(tr("Other Options"), this);
        other->setWindowOpacity(0.8);
        other->setStyleSheet(Utils::setStyle("contextmenu"));

        /** Visualização */
        QMenu visualization(tr("Visualizations"), this);
//        visualization.setGraphicsEffect(effect3);
        visualization.setStyleSheet(Utils::setStyle("contextmenu"));

        /** Opções de vídeo */
        QMenu optionVideo(tr("Video Options"), this);
//        optionVideo.setGraphicsEffect(effect4);
        optionVideo.setStyleSheet(Utils::setStyle("contextmenu"));

        /** Modo repetição */
        QAction repeat(tr("Repeat Mode"), this);
        repeat.setShortcut(QKeySequence(CTRL | ALT | Qt::Key_T));
        if (mediaPlayer->repeat() == 0) Utils::changeMenuIcon(repeat, "repeat");
        else Utils::changeMenuIcon(repeat, "repeat-on");
        connect(&repeat, SIGNAL(triggered()), SLOT(setRepeat()));

        /** Velocidade de execução */
        QAction speed(tr("Speed"), this);

        /** Dimensão de exibição */
        QAction aspectratio(tr("Aspect Ratio"), this);

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
        connect(&settings, SIGNAL(triggered()), SLOT(setSettings()));

        /** Menu sobre */
        QAction mabout(tr("About"), this);
        Utils::changeMenuIcon(mabout, "about");
        connect(&mabout, SIGNAL(triggered()), SLOT(setAbout()));

        /** Montagem do menu de opções de vídeo */
        optionVideo.addAction(&aspectratio);

        /** Montagem do menu para outras opções */
        other->addMenu(channel);
//        other->addAction(&speed);
        other->addAction(&repeat);

        /** Montagem do menu principal */
        contextMenu->addAction(&open);
        contextMenu->addSeparator();
        contextMenu->addAction(&fullscreen);
        contextMenu->addSeparator();
        contextMenu->addAction(&shuffle);
        contextMenu->addAction(&replay);
        contextMenu->addSeparator();
//        contextMenu->addMenu(&visualization);
//        contextMenu->addMenu(&optionVideo);
        contextMenu->addMenu(other);
        contextMenu->addSeparator();
        contextMenu->addAction(&settings);
        contextMenu->addAction(&mabout);
        contextMenu->exec(mapToGlobal(pos));
    }
}
