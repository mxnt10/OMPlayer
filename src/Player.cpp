#include <QGraphicsOpacityEffect>
#include <QMenu>
#include <QRandomGenerator>
#include <QToolTip>

#include <MediaInfoDLL.h>
#include <ScreenSaver>

#include <filesystem>
#include <xcb/xcb.h>

#include "Defines.h"
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


    /** Fundo preto */
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::black);
    this->setPalette(pal);


    /** Janelas de configurações do programa e preview */
    about = new About(this);
    sett = new Settings(this);
    preview = new VideoPreviewWidget(this);
    preview->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    connect(sett, SIGNAL(emitvalue(QString)), this, SLOT(setRenderer(QString)));
    connect(sett, SIGNAL(emitclose()), this, SLOT(closeSettings()));
    connect(about, SIGNAL(emitclose()), this, SLOT(closeAbout()));


    /** Parte principal do programa que permite o funcionamento do reprodutor */
    mediaPlayer = new AVPlayer(this);
    setRenderer(JsonTools::readJson("renderer"));
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
    connect(mediaPlayer, SIGNAL(notifyIntervalChanged()), SLOT(updateSliderUnit()));
    connect(mediaPlayer, SIGNAL(started()), SLOT(onStart()));
    connect(mediaPlayer, SIGNAL(stopped()), SLOT(onStop()));
    connect(mediaPlayer, SIGNAL(paused(bool)), SLOT(onPaused(bool)));
    connect(mediaPlayer, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)), SLOT(onMediaStatusChanged()));
    connect(mediaPlayer, SIGNAL(error(QtAV::AVError)), SLOT(handleError(QtAV::AVError)));


    /** Playlist do reprodutor */
    playlist = new PlayList();
    playlist->load();
    connect(playlist, SIGNAL(aboutToPlay(QString)), SLOT(doubleplay(QString)));
    connect(playlist, SIGNAL(firstPlay(QString,int)), SLOT(firstPlay(QString,int)));
    connect(playlist, SIGNAL(selected(int)), SLOT(setSelect(int)));
    connect(playlist, SIGNAL(emithiden()), SLOT(setHide()));
    connect(playlist, SIGNAL(emithide()), SLOT(hideTrue()));
    connect(playlist, SIGNAL(emitnohide()), SLOT(hideFalse()));
    connect(playlist, SIGNAL(emitstop()), SLOT(setStop()));
    connect(playlist, SIGNAL(emitremove(int)), SLOT(ajustActualItem(int)));


    /** Barra de progresso de execução */
    slider = new Slider(this, true, 28, 0);
    slider->setTracking(true);
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
    playBtn = new Button("play", 48);
    connect(playBtn, SIGNAL(clicked()), SLOT(playPause()));
    connect(playBtn, SIGNAL(emitEnter()), SLOT(hideFalse()));
    stopBtn = new Button("stop", 32);
    connect(stopBtn, SIGNAL(clicked()), SLOT(setStop()));
    connect(stopBtn, SIGNAL(emitEnter()), SLOT(hideFalse()));
    nextBtn = new Button("next", 32);
    connect(nextBtn, SIGNAL(clicked()), SLOT(Next()));
    connect(nextBtn, SIGNAL(emitEnter()), SLOT(hideFalse()));
    previousBtn = new Button("previous", 32);
    connect(previousBtn, SIGNAL(clicked()), SLOT(Previous()));
    connect(previousBtn, SIGNAL(emitEnter()), SLOT(hideFalse()));
    replayBtn = new Button("replay", 32);
    connect(replayBtn, SIGNAL(clicked()), SLOT(setReplay()));
    connect(replayBtn, SIGNAL(emitEnter()), SLOT(hideFalse()));
    shuffleBtn = new Button("shuffle", 32);
    connect(shuffleBtn, SIGNAL(clicked()), SLOT(setShuffle()));
    connect(shuffleBtn, SIGNAL(emitEnter()), SLOT(hideFalse()));


    /** Assistentes para mapear quando a ocultação dos controles não deve ser feita */
    qDebug("%s(%sDEBUG%s):%s Preparando o layout da interface ...\033[0m", GRE, RED, GRE, CYA);
    auto *enterfilter = new EventFilter(this, 3);
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
    buttons->addWidget(replayBtn);
    buttons->addWidget(shuffleBtn);
    buttons->addSpacing(5);
    buttons->addWidget(line);
    buttons->addSpacing(5);
    buttons->addWidget(previousBtn);
    buttons->addWidget(playBtn);
    buttons->addWidget(stopBtn);
    buttons->addWidget(nextBtn);
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
    logo->setPixmap(QPixmap(Utils::setIcon(true)));


    /** Layout principal criado usando sobreposição de widgets */
    layout = new QGridLayout();
    layout->setMargin(0);
    layout->addWidget(video->widget(), 0, 0);
    layout->addWidget(logo, 0, 0);
    this->setLayout(layout);


    /** Filtro de eventos */
    filter = new EventFilter(this, 1);
    auto *cfilter = new EventFilter(this, 2);
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
    connect(filter, SIGNAL(emitSettings()), SLOT(setSettings()));
    connect(filter, SIGNAL(emitLeave()), SLOT(setHide()));
    connect(cfilter, SIGNAL(emitLeave()), SLOT(setHide()));


    /** Temporizador para o cálculo do número de cliques em 300ms */
    click = new QTimer(this);
    click->setSingleShot(true);
    click->setInterval(300);
    connect(click, SIGNAL(timeout()), this, SLOT(detectClick()));
}


/** Destrutor */
OMPlayer::~OMPlayer() = default;


/**********************************************************************************************************************/


/** Função para abrir arquivos multimídia */
void OMPlayer::openMedia(const QStringList &parms) {
    playlist->addItems(parms);
}


/** Setando opções de Rendenização */
void OMPlayer::setRenderer(const QString &op) {
    VideoRenderer *vo;

    if (QString::compare(op, "openglwidget", Qt::CaseSensitive) == 0)
        vo = VideoRenderer::create(VideoRendererId_OpenGLWidget);

    if (QString::compare(op, "qglwidget2", Qt::CaseSensitive) == 0)
        vo = VideoRenderer::create(VideoRendererId_GLWidget2);

    if (QString::compare(op, "widget", Qt::CaseSensitive) == 0)
        vo = VideoRenderer::create(VideoRendererId_Widget);

    if (QString::compare(op, "xvideo", Qt::CaseSensitive) == 0)
        vo = VideoRenderer::create(VideoRendererId_XV);

    if (QString::compare(op, "x11", Qt::CaseSensitive) == 0)
        vo = VideoRenderer::create(VideoRendererId_X11);


    /** Verificando se o layout já foi criado. É necessário readicionar o widget ao mudar a renderização. */
    if (layout) layout->removeWidget(video->widget());

    video = new VideoOutput(vo->id(), this);
    video->widget()->setMouseTracking(true);
    mediaPlayer->setRenderer(video);

    /** Readicionando o widget ao layout */
    if (layout) layout->addWidget(video->widget(), 0, 0);

    const VideoRendererId vid = mediaPlayer->renderer()->id();
    if (vid == VideoRendererId_XV || vid == VideoRendererId_GLWidget2 || vid == VideoRendererId_OpenGLWidget)
        mediaPlayer->renderer()->forcePreferredPixelFormat(true);
    else
        mediaPlayer->renderer()->forcePreferredPixelFormat(false);

    qDebug("%s(%sDEBUG%s):%s Setando rendenização %s %i ...\033[0m", GRE, RED, GRE, BLU, qUtf8Printable(op), vo->id());
}


/** Rodando as funções após aberto outras instâncias */
void OMPlayer::onLoad() {
    playlist->load(true);
    playlist->selectCurrent(actualitem);
}


/** Função que possui o propósito de ajustar a ordem de execução ao remover itens da playlist anteriores ao
 * arquivo multimídia sendo reproduzido no momento */
void OMPlayer::ajustActualItem(int item) {
    int actual = nextitem - 1;
    int c = 0;
    if (item < actual) {
        c++;
        playlist->selectCurrent(nextitem - c);
        nextitem--;
        actualitem--;
        previousitem--;
        if (previousitem == -1)
            previousitem = playlist->setListSize() - 1;
        if (actualitem == -1)
            actualitem = playlist->setListSize() - 1;
        if (nextitem == -1)
            nextitem = playlist->setListSize() - 1;
    }

    if (item > actual)
        playlist->selectCurrent(actual);

    if (item == actual)
        play(playlist->getItems(nextitem), nextitem);

    listnum.clear();
}


/** Setando o item atualmente selecionado */
void OMPlayer::setSelect(int item) {
    actualitem = item;
    if (!playing) {
        qDebug("%s(%sDEBUG%s):%s Selecionando o item %s manualmente ...\033[0m", GRE, RED, GRE, ORA,
               qUtf8Printable(Utils::mediaTitle(playlist->getItems(item))));
    }
}


/** Função geral para execução de arquivos multimídia */
void OMPlayer::play(const QString &isplay, int index) {
    this->setWindowTitle(Utils::mediaTitle(isplay));
    if (mediaPlayer->isPlaying() || mediaPlayer->isPaused())
        mediaPlayer->stop();
    mediaPlayer->play(isplay);
    if (index > (-1))
        playlist->selectCurrent(index);
}


/** Para executar os itens recém adicionados da playlist */
void OMPlayer::firstPlay(const QString &isplay, int pos) {
    if (!mediaPlayer->isPlaying()) {
        qDebug("%s(%sDEBUG%s):%s Reproduzindo um Arquivo Multimídia ...\033[0m", GRE, RED, GRE, ORA);
        play(isplay, pos);

        actualitem = pos;
        nextitem = actualitem + 1;
        previousitem = actualitem - 1;
        if (actualitem == 0)
            previousitem = playlist->setListSize() - 1;
        if (actualitem == playlist->setListSize() - 1)
            nextitem = 0;
    }
}


/** Executa um item selecionado da playlist */
void OMPlayer::doubleplay(const QString &name) {
    qDebug("%s(%sDEBUG%s):%s Reproduzindo um Arquivo Multimídia ...\033[0m", GRE, RED, GRE, ORA);
    play(name);

    /**
     * Cálculo dos próximos itens a serem executados. O cálculo foi baseado na seguinte forma, uma lista começa
     * do 0. Portanto, uma lista de 10 itens vai vai de 0 a 9.
     *
     * O primeiro item selecionado da playlist vai ser 0, e o item anterior vai ser contabilizado como -1, que não
     * existe na lista. Por isso, é necessário o ajuste.
     *
     * A função setListSize() retorna o número de itens e não a posição, necessário subtrair 1.
     *
     * Já ao selecionar o último número da playlist, o próximo item vai exceder o número de posições e como a ideia
     * é que a função Next() ao chegar ao último item retorne para o ínicio, então o valor é corrigido para 0.
     */

    actualitem = playlist->selectItems();
    nextitem = actualitem + 1;
    previousitem = actualitem - 1;
    if (actualitem == 0)
        previousitem = playlist->setListSize() - 1;
    if (actualitem == playlist->setListSize() - 1)
        nextitem = 0;
}


/** Função para selecionar aleatóriamente a próxima mídia */
void OMPlayer::nextRand() {
    qDebug("%s(%sDEBUG%s):%s Reproduzindo uma mídia aleatória ...\033[0m", GRE, RED, GRE, ORA);
    if (listnum.size() < playlist->setListSize() - 1) {
        actualitem = QRandomGenerator::global()->bounded(playlist->setListSize() - 1);

        /**
         * No método aleatório convencional, alguns itens são reproduzidos mais vezes que os outros. Portanto,
         * criei esse método para executar os itens aleatoriamente só que um de cada vez. Assim, quando todos os
         * arquivos multimídia forem reproduzidos aleatoriamente, aí sim é zerado o contador para recameçar a
         * execução aleatória dos itens denovo.
         */

        while (!listnum.filter(QRegExp("^(" + QString::number(actualitem) + ")$")).isEmpty())
            actualitem = QRandomGenerator::global()->bounded(playlist->setListSize());

        listnum.append(QString::number(actualitem));
    } else {
        actualitem = QRandomGenerator::global()->bounded(playlist->setListSize());
        listnum.clear();
        listnum.append(QString::number(actualitem));
    }

    play(playlist->getItems(actualitem), actualitem);

    /** Cálculo dos próximos itens a serem executados */
    nextitem = actualitem + 1;
    previousitem = actualitem - 1;
    if (actualitem == 0)
        previousitem = playlist->setListSize() - 1;
    if (actualitem == playlist->setListSize() - 1)
        nextitem = 0;
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

        /** Cálculo dos próximos itens a serem executados */
        nextitem++;
        actualitem++;
        previousitem++;
        /** O valor dos índices é sempre o número de itens - 1, se igualar tem que zerar */
        if (previousitem == playlist->setListSize())
            previousitem = 0;
        if (actualitem == playlist->setListSize())
            actualitem = 0;
        if (nextitem == playlist->setListSize())
            nextitem = 0;
    }
}


/** Botão previous */
void OMPlayer::Previous() {
    if (mediaPlayer->isPlaying()) {
        qDebug("%s(%sDEBUG%s):%s Reproduzindo um item anterior ...\033[0m", GRE, RED, GRE, ORA);
        play(playlist->getItems(previousitem), previousitem);

        /** Cálculo dos próximos itens a serem executados */
        nextitem--;
        actualitem--;
        previousitem--;
        /** Dispensa comentários, o valor dos índices é sempre o número de itens - 1 */
        if (previousitem == -1)
            previousitem = playlist->setListSize() - 1;
        if (actualitem == -1)
            actualitem = playlist->setListSize() - 1;
        if (nextitem == -1)
            nextitem = playlist->setListSize() - 1;
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
        nextitem = actualitem + 1;
        previousitem = actualitem - 1;
        if (actualitem == 0)
            previousitem = playlist->setListSize() - 1;
        if (actualitem == playlist->setListSize() - 1)
            nextitem = 0;
        return;
    }
    mediaPlayer->pause(!mediaPlayer->isPaused());
}


/** Função para o botão stop */
void OMPlayer::setStop() {
    playing = false;
    actualitem = playlist->selectItems();
    mediaPlayer->stop();
}


/** Função para alterar o botão play/pause */
void OMPlayer::onPaused(bool paused) {
    if (paused) {
        qDebug("%s(%sDEBUG%s):%s Pausando ...\033[0m", GRE, RED, GRE, ORA);
        Utils::changeIcon(playBtn, "play");
        playBtn->setToolTip("Play");
    } else {
        qDebug("%s(%sDEBUG%s):%s Reproduzindo ...\033[0m", GRE, RED, GRE, ORA);
        Utils::changeIcon(playBtn, "pause");
        playBtn->setToolTip("Pause");
    }
}


/** Função que define alguns parâmetros ao iniciar a reprodução */
void OMPlayer::onStart() {
    logo->setVisible(false);
    slider->setDisabled(false);
    playing = true;

    Utils::changeIcon(playBtn, "pause");
    playBtn->setToolTip("Pause");

    /** Definindo dimensões para o preview */
    MI.Open(mediaPlayer->file().toStdString());
    Width = MI.Get(Stream_Video, 0, "Width", Info_Text, Info_Name).c_str();
    Height = MI.Get(Stream_Video, 0, "Height", Info_Text, Info_Name).c_str();
    MI.Close();

    /** Atualizando o status do item da playlist se necessário */
    if (playlist->setDuration() == 0) {
        int row = playlist->selectItems();
        QString url = mediaPlayer->file();
        qint64 duration = mediaPlayer->mediaStopPosition();

        MI.Open(url.toStdString());
        QString format = MI.Get(Stream_General, 0, "Format", Info_Text, Info_Name).c_str();
        MI.Close();

        playlist->removeSelectedItems(true);
        playlist->insert(url, row, duration, format);
        playlist->save();
    }

    /** Definindo o tempo de duração no slider */
    slider->setMinimum(int(mediaPlayer->mediaStartPosition()));
    slider->setMaximum(int(mediaPlayer->mediaStopPosition() - Utils::setDifere(unit))/ unit);
    end->setText(QTime(0, 0, 0).addMSecs(int(mediaPlayer->mediaStopPosition())).toString(QString::fromLatin1("HH:mm:ss")));

    /** Correção dos itens adicionados */
    if (playlist->setListSize() == 1)
        previousitem = nextitem = 0;

    /** Desativando Bloqueio de tela */
    if (!isblock) {
        ScreenSaver::instance().active();
        isblock = true;
    }

    updateSlider(mediaPlayer->position());
}


/** Função que define alguns parâmetros ao parar a reprodução */
void OMPlayer::onStop() {
    if (!playing) {
        qDebug("%s(%sDEBUG%s):%s Finalizando a Reprodução ...\033[0m", GRE, RED, GRE, ORA);
        this->setWindowTitle(QString(PRG_NAME));

        Utils::changeIcon(playBtn, "play");
        playBtn->setToolTip("Play");

        slider->setMaximum(0);
        slider->setDisabled(true);
        logo->setVisible(true);
        current->setText(QString::fromLatin1("-- -- : -- --"));
        end->setText(QString::fromLatin1("-- -- : -- --"));
        onTimeSliderLeave();

        /** Reativando Bloqueio de tela */
        if (isblock) {
            ScreenSaver::instance().deactive();
            isblock = false;
        }
    }
}


/** Ativação do modo repetição */
void OMPlayer::setReplay() {
    if (!restart) {
        restart = true;
        Utils::changeIcon(replayBtn, "replay-on");
    } else {
        restart = false;
        Utils::changeIcon(replayBtn, "replay");
    }
}


/** Ativação do modo aleatório */
void OMPlayer::setShuffle() {
    if (!randplay) {
        randplay = true;
        Utils::changeIcon(shuffleBtn, "shuffle-on");
    } else {
        randplay = false;
        Utils::changeIcon(shuffleBtn, "shuffle");
    }
}


/** Função para ativar a função pause com um clique */
void OMPlayer::enablePause() {
    pausing = true;
}


/** Contador para mapear o clique único */
void OMPlayer::clickCount() {
    if (!click->isActive())
        click->start();
    count = count + 1; /** Contador de cliques */
}


/** Função que mapeia um único clique e executa as ações de pausar e executar */
void OMPlayer::detectClick() {
    if (prevent) count = 0;
    if (count == 1 && !enterpos && playing && pausing)
        playPause();
    count = 0;
    pausing = prevent = false;
    if (wctl->isActiveWindow())
        QCursor::setPos(QCursor::pos() + QPoint(1, 1));
}


/** Gerenciar tela cheia */
void OMPlayer::changeFullScreen() {
    if (prevent) return;
    if (this->isFullScreen())
        leaveFullScreen();
    else
        enterFullScreen();
}


/** Entrar no modo tela cheia */
void OMPlayer::enterFullScreen() {
    qDebug("%s(%sDEBUG%s):%s Entrando no Modo Tela Cheia ...\033[0m", GRE, RED, GRE, ORA);

    if (this->isMaximized()) /** Mapear interface maximizada */
        maximize = true;

    this->showFullScreen();
    wctl->close();
    filter->setMove(false);
    enterpos = false;
}


/** Sair do modo tela cheia */
void OMPlayer::leaveFullScreen() {
    qDebug("%s(%sDEBUG%s):%s Saindo do Modo Tela Cheia ...\033[0m", GRE, RED, GRE, ORA);
    this->showNormal();

    /**
     * Precisa caso a interface já estava maximizada antes. Note que showMaximized() só funciona após a execução
     * de showNormal(), a tela não pula direto de fulscreen para maximizado.
     */
    if (maximize)
        this->showMaximized();

    wctl->close();
    filter->setMove(false);
    enterpos = maximize = false;
}


/** Função auxiliar para fechar os controles e a playlist */
void OMPlayer::setHide() {
    wctl->close();
    hideTrue();
    filter->setMove(false);
}


/** Função auxiliar para abrir os controles e a playlist */
void OMPlayer::setShow() {
    if (!showsett) {
        wctl->show();
        wctl->activateWindow();
    }
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


/** Função que fecha as configurações ao receber a emissão */
void OMPlayer::closeSettings() {
    showsett = false;
    if (!this->isFullScreen())
        prevent = true;
    this->setMaximumSize(screen);
    this->setMinimumSize(min);
    filter->setSett(showsett);
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


/** Função que fecha o sobre ao receber a emissão */
void OMPlayer::closeAbout() {
    showsett = false;
    if (!this->isFullScreen())
        prevent = true;
    this->setMaximumSize(screen);
    this->setMinimumSize(min);
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


/** Pré-visualização ao posicionar o mouse no slider */
void OMPlayer::onTimeSliderHover(int pos, int value) {
    int fixV = 72;
    QPoint gpos = slider->pos();

    /** Definição da posição na tela e exibição do tooltip */
    QPoint gpos1 = mapToGlobal(gpos + QPoint(pos + 12, 5));
    QPoint gpos2 = mapToGlobal(gpos + QPoint(pos, -7));
    QToolTip::showText(gpos1, QTime(0, 0, 0).addMSecs(value * unit).toString(QString::fromLatin1("HH:mm:ss")), this);

    /** Arquivos de áudio não são vídeos */
    if (mediaPlayer->currentVideoStream() == (-1) || Width.isEmpty() || Height.isEmpty()) return;
    int setX = Utils::calcX(fixV, Width.toInt(), Height.toInt());

    /** Exibição da pré-visualização */
    preview->setFile(mediaPlayer->file());
    preview->setTimestamp(value * unit);
    preview->preview();
    preview->resize(setX, fixV);
    preview->move(gpos2 - QPoint(setX/2, fixV));
    preview->show();
}


/** Apenas para exibição do debug */
void OMPlayer::onTimeSliderEnter() {
    if (playing && !Width.isEmpty() && !Height.isEmpty())
        qDebug("%s(%sDEBUG%s):%s Exibindo a pré-visualização ...\033[0m", GRE, RED, GRE, CYA);
    hideFalse();
}


/** Saindo da pré-visualização */
void OMPlayer::onTimeSliderLeave() {
    if (preview->isVisible()) {
        qDebug("%s(%sDEBUG%s):%s Finalizando a pré-visualização ...\033[0m", GRE, RED, GRE, CYA);
        preview->close();
    }
    hideTrue();
}


/** Altera o tempo de execução ao pressionar ou mover a barra de progresso de execução */
void OMPlayer::seekBySlider(int value) {
    if (!mediaPlayer->isPlaying()) return;
    mediaPlayer->setSeekType(QtAV::AccurateSeek);
    mediaPlayer->seek(qint64(qint64(value) * unit));
}


/** Altera o tempo de execução ao pressionar a barra de progresso de execução */
void OMPlayer::seekBySlider() {
    seekBySlider(slider->value());
}


/** Função para atualizar a barra de progresso de execução */
void OMPlayer::updateSlider(qint64 value) {
    if (mediaPlayer->isSeekable())
        slider->setValue(int(value / unit));
    current->setText(QTime(0, 0, 0).addMSecs(int(value)).toString(QString::fromLatin1("HH:mm:ss")));

    /** Próxima mídia */
    if (int(value) > mediaPlayer->duration() - Utils::setDifere(unit)) {
        if (actualitem == playlist->setListSize() - 1 && !restart && !randplay) {
            playing = false;
            mediaPlayer->stop();
            playlist->selectClean();
            previousitem = playlist->setListSize() - 1;
            actualitem = 0;
            nextitem = 1;
        } else
            Next();
    }
}


/** Função para atualizar a barra de progresso de execução */
void OMPlayer::updateSliderUnit() {
    unit = mediaPlayer->notifyInterval();
    slider->setMaximum(int(mediaPlayer->mediaStopPosition() - Utils::setDifere(unit))/ unit);
    updateSlider(mediaPlayer->position());
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


/** Mapeamento de eventos apenas para executar um ação após fechar os diálogos */
bool OMPlayer::nativeEvent(const QByteArray &eventType, void *message, long *result) {
    Q_UNUSED(result);
    if (eventType == "xcb_generic_event_t") {
        auto* event = static_cast<xcb_generic_event_t *>(message);
        if (event->response_type == 35) {
            if (prevent) {
                qDebug("%s(%sDEBUG%s):%s Gerando click fake ...\033[0m", GRE, RED, GRE, RED);
                system("xdotool click 1");
            }
        }
    }
    return false;
}


/** Mapeamento dos eventos de maximizar, minimizar e restaurar */
void OMPlayer::changeEvent(QEvent *event) {
    if(event->type() == QEvent::WindowStateChange) {
        if (int(this->windowState()) == 0) {      // Restaurar
            qDebug("%s(%sDEBUG%s):%s Restaurando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, ORA);
        }
        else if (int(this->windowState()) == 1) { // Minimização
            qDebug("%s(%sDEBUG%s):%s Minimizando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, ORA);
            if (showsett) this->showNormal();
        }
        else if (int(this->windowState()) == 2) { // Maximização
            qDebug("%s(%sDEBUG%s):%s Maximizando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, ORA);
        }
        else if (int(this->windowState()) == 3) { // Minimização direta
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
    qDebug("%s(%sDEBUG%s):%s Iniciando o Menu de Contexto ...\033[0m", GRE, RED, GRE, CYA);
    auto *effect = new QGraphicsOpacityEffect();
    effect->setOpacity(0.8);
    Utils::arrowMouse();

    QMenu contextMenu(tr("Context menu"), this);
    contextMenu.setGraphicsEffect(effect);
    contextMenu.setStyleSheet(Utils::setStyle("contextmenu"));

    /** Menu de abrir */
    QAction open("Open Files", this);
    open.setShortcut(QKeySequence(CTRL | Qt::Key_O));
    Utils::changeMenuIcon(open, "folder");
    connect(&open, SIGNAL(triggered()), SLOT(openMedia()));

    /** Menu tela cheia */
    QAction fullscreen("Show Fullscreen", this);
    if (this->isFullScreen())
        fullscreen.setText("Exit Fullscreen");
    fullscreen.setShortcut(QKeySequence(ALT | Qt::Key_Enter));
    Utils::changeMenuIcon(fullscreen, "fullscreen");
    connect(&fullscreen, SIGNAL(triggered()), SLOT(changeFullScreen()));

    /** Menu aleatório */
    QAction shuffle("Shuffle", this);
    shuffle.setShortcut(QKeySequence(CTRL | Qt::Key_H));
    Utils::changeMenuIcon(shuffle, "shuffle-menu");
    connect(&shuffle, SIGNAL(triggered()), SLOT(setShuffle()));

    /** Menu repetir */
    QAction replay("Replay", this);
    replay.setShortcut(QKeySequence(CTRL | Qt::Key_T));
    Utils::changeMenuIcon(replay, "replay-menu");
    connect(&replay, SIGNAL(triggered()), SLOT(setReplay()));

    /** Menu de configuração */
    QAction settings("Settings", this);
    settings.setShortcut(QKeySequence(ALT | Qt::Key_S));
    Utils::changeMenuIcon(settings, "settings");
    connect(&settings, SIGNAL(triggered()), SLOT(setSettings()));

    /** Menu sobre */
    QAction mabout("About", this);
    Utils::changeMenuIcon(mabout, "about");
    connect(&mabout, SIGNAL(triggered()), SLOT(setAbout()));

    /** Montagem do menu */
    contextMenu.addAction(&open);
    contextMenu.addSeparator();
    contextMenu.addAction(&fullscreen);
    contextMenu.addSeparator();
    contextMenu.addAction(&shuffle);
    contextMenu.addAction(&replay);
    contextMenu.addSeparator();
    contextMenu.addAction(&settings);
    contextMenu.addAction(&mabout);
    contextMenu.exec(mapToGlobal(pos));
}
