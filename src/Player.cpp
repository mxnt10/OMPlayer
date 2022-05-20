#include <QCloseEvent>
#include <QFileDialog>
#include <QLayout>
#include <QGraphicsEffect>
#include <QGuiApplication>
#include <QMenu>
#include <QRandomGenerator>
#include <QScreen>
#include <QShortcut>
#include <QSpacerItem>
#include <QStyle>
#include <QTimer>
#include <QToolTip>
#include <QWidget>

#include <MediaInfoDLL.h>
#include <ScreenSaver>

#include "Button.h"
#include "Defines.h"
#include "Label.h"
#include "Player.h"
#include "PlayList.h"
#include "Settings.h"
#include "Slider.h"
#include "Utils.h"
#include "Widget.h"

using namespace MediaInfoDLL;


/**********************************************************************************************************************/


/** Construtor que define a interface do programa */
VideoPlayer::VideoPlayer(QWidget *parent) : QWidget(parent),
    contextmenu(false),
    enterpos(false),
    isblock(false),
    maximize(false),
    moving(false),
    pausing(false),
    playing(false),
    randplay(false),
    restart(false),
    showsett(false),
    actualitem(0),
    nextitem(0),
    previousitem(0),
    count(0),
    mUnit(500),
    sett(nullptr),
    preview(nullptr),
    Width("192"),
    Height("108") {
    qDebug("%s(%sDEBUG%s):%s Iniciando o Reprodutor Multimídia ...\033[0m", GRE, RED, GRE, CYA);
    this->setWindowTitle(QString(PRG_NAME));
    this->setWindowIcon(QIcon(Utils::setIcon()));
    this->setStyleSheet(Utils::setStyle("global"));
    this->setMinimumSize(906, 510);
    this->setMouseTracking(true); /** Mapeamento do mouse */
    this->move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center());
    theme = "circle";


    /** Habilitando o menu de contexto */
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(ShowContextMenu(QPoint)));


    /** Parte principal do programa que permite o funcionamento do reprodutor */
    qDebug("%s------------------------------%s", RED, BLU);
    video = new VideoOutput(VideoRendererId_OpenGLWidget, this);
    mediaPlayer = new AVPlayer(this);
    mediaPlayer->setRenderer(video);
    mediaPlayer->renderer()->forcePreferredPixelFormat(false);
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
    connect(mediaPlayer, SIGNAL(notifyIntervalChanged()), SLOT(updateSliderUnit()));
    connect(mediaPlayer, SIGNAL(started()), SLOT(onStart()));
    connect(mediaPlayer, SIGNAL(stopped()), SLOT(onStop()));
    connect(mediaPlayer, SIGNAL(paused(bool)), SLOT(onPaused(bool)));
    qDebug("%s------------------------------\033[0m", RED);


    /** Playlist do reprodutor */
    playlist = new PlayList(this);
    playlist->setSaveFile(QDir::homePath() + "/.config/OMPlayer/playlist.qds");
    playlist->load();
    connect(playlist, SIGNAL(aboutToPlay(QString)), SLOT(doubleplay(QString)));
    connect(playlist, SIGNAL(firstPlay(QString)), SLOT(firstPlay(QString)));
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
    connect(slider, SIGNAL(emitEnter()), SLOT(hideFalse()));
    connect(slider, SIGNAL(emitLeave()), SLOT(hideTrue()));


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


    /** Teclas de atalho */
    auto *keyCtrlA = new QShortcut(this);
    keyCtrlA->setKey(QKeySequence(CTRL | Qt::Key_A));
    connect(keyCtrlA, SIGNAL(activated()), SLOT(openMedia()));
    auto *keyCtrlO = new QShortcut(this);
    keyCtrlO->setKey(QKeySequence(CTRL | Qt::Key_O));
    connect(keyCtrlO, SIGNAL(activated()), SLOT(openMedia()));
    auto *keyAltEnter = new QShortcut(this);
    keyAltEnter->setKey(QKeySequence(ALT | ENTER));
    connect(keyAltEnter, SIGNAL(activated()), SLOT(changeFullScreen()));
    auto *keyEsc = new QShortcut(this);
    keyEsc->setKey(QKeySequence(ESC));
    connect(keyEsc, SIGNAL(activated()), SLOT(leaveFullScreen()));
    auto *keyCtrlH = new QShortcut(this);
    keyCtrlH->setKey(QKeySequence(CTRL | Qt::Key_H));
    connect(keyCtrlH, SIGNAL(activated()), SLOT(setShuffle()));
    auto *keyCtrlT = new QShortcut(this);
    keyCtrlT->setKey(QKeySequence(CTRL | Qt::Key_T));
    connect(keyCtrlT, SIGNAL(activated()), SLOT(setReplay()));


    /** Opacidade de 80% para os widgets de fundo dos controles e playlist */
    qDebug("%s(%sDEBUG%s):%s Preparando o layout da interface ...\033[0m", GRE, RED, GRE, CYA);
    auto *effect1 = new QGraphicsOpacityEffect();
    effect1->setOpacity(OPACY);
    auto *effect2 = new QGraphicsOpacityEffect();
    effect2->setOpacity(OPACY);


    /** Assistentes para mapear quando a ocultação dos controles não deve ser feita */
    auto *nohide1 = new Widget();
    connect(nohide1, SIGNAL(emitEnter()), SLOT(hideFalse()));
    connect(nohide1, SIGNAL(emitLeave()), SLOT(hideTrue()));
    auto *nohide2 = new Widget();
    connect(nohide2, SIGNAL(emitEnter()), SLOT(hideFalse()));
    connect(nohide2, SIGNAL(emitLeave()), SLOT(hideTrue()));
    auto *nohide3 = new Widget();
    connect(nohide3, SIGNAL(emitEnter()), SLOT(hideFalse()));
    connect(nohide3, SIGNAL(emitLeave()), SLOT(hideTrue()));


    /** Plano de fundo semitransparente da função about */
    auto *qabout = new QWidget();
    qabout->setFixedSize(856, 460);
    qabout->setGraphicsEffect(effect2);
    qabout->setStyleSheet(Utils::setStyle("widget"));


    /** Ajuste do layout do about */
    auto *vlabout = new QVBoxLayout();
    vlabout->addWidget(qabout);
    vlabout->setAlignment(CENTER);


    /** Nome do programa e descrição */
    auto *description = new Label(TOP, 0, nullptr, About::getDescription());
    auto *name = new Label(TOP, 0, nullptr, PRG_DESC);
    name->setMaximumHeight(40);
    name->setStyleSheet("font-size: 24pt");


    /** Definição do icon logo */
    auto *iconlogo = new Label(CENTER);
    iconlogo->setMinimumHeight(200);
    iconlogo->setPixmap(QPixmap(Utils::setIcon()));


    /** Versão do programa e demais informações*/
    auto *version = new Label(RIGHT, 0, nullptr, "Version " + QString::fromStdString(VERSION));
    auto *maintainer = new Label(BOTTON, 0, nullptr, About::getTextMaintainer());
    version->setStyleSheet("font-size: 12pt");
    maintainer->setStyleSheet("font-size: 12pt");


    /** Organizaçãodas informações do Sobre */
    auto *labout = new QGridLayout();
    labout->setMargin(20);
    labout->addWidget(name, 0, 0);
    labout->addWidget(version, 0, 1);
    labout->addWidget(description, 1, 0, 1, 2);
    labout->addWidget(iconlogo, 2, 0, 1, 2);
    labout->addWidget(maintainer, 3, 0);


    /** Montando a interface do Sobre */
    about = new QWidget();
    auto *gabout = new QGridLayout(about);
    gabout->addLayout(vlabout, 0, 0);
    gabout->addLayout(labout, 0, 0);


    /** Plano de fundo semitransparente dos controles de reprodução */
    auto *bgcontrol = new QWidget();
    bgcontrol->setMaximumHeight(120);
    bgcontrol->setGraphicsEffect(effect1);
    bgcontrol->setStyleSheet(Utils::setStyle("widget"));


    /** Não tem como mapear a posição do mouse sem isso */
    auto *mpos = new QWidget();
    mpos->setMouseTracking(true);


    /** Ajuste do plano de fundo dos controles */
    auto *bgctl = new QGridLayout();
    bgctl->setMargin(10);
    bgctl->addWidget(bgcontrol, 0, 0);
    bgctl->addWidget(nohide3, 0, 0);


    /** Gerando uma linha */
    auto line = new QFrame();
    line->setFixedWidth(2);
    line->setFixedHeight(25);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background: #cccccc");


    /** Layout dos botões */
    auto *buttons = new QHBoxLayout();
    buttons->setSpacing(5);
    buttons->addWidget(nohide1);
    buttons->addWidget(replayBtn);
    buttons->addWidget(shuffleBtn);
    buttons->addSpacing(5);
    buttons->addWidget(line);
    buttons->addSpacing(5);
    buttons->addWidget(previousBtn);
    buttons->addWidget(playBtn);
    buttons->addWidget(stopBtn);
    buttons->addWidget(nextBtn);
    buttons->addWidget(nohide2);


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


    /** Conteiner que permite o mapeamento do mouse, ocultação e exibição dos controles */
    wctl = new QWidget();
    wctl->setMouseTracking(true);


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
    auto *layout = new QGridLayout();
    layout->setMargin(0);
    layout->addWidget(video->widget(), 0, 0);
    layout->addWidget(logo, 0, 0);
    layout->addWidget(mpos, 0, 0);
    layout->addWidget(about, 0, 0);
    layout->addWidget(wctl, 0, 0);
    this->setLayout(layout);
    about->setVisible(false);
    wctl->setVisible(false);


    /** Temporizador para o cálculo do número de cliques em 300ms */
    click = new QTimer(this);
    click->setSingleShot(true);
    click->setInterval(300);
    connect(click, SIGNAL(timeout()), this, SLOT(detectClick()));
}


/** Destrutor */
VideoPlayer::~VideoPlayer() = default;


/**********************************************************************************************************************/


/** Função para abrir arquivos multimídia */
void VideoPlayer::openMedia(const QStringList &parms) {
    playlist->addItems(parms);
}


/** Rodando as funções após aberto outras instâncias */
void VideoPlayer::runLoad() {
    playlist->load(true);
    playlist->selectCurrent(actualitem);
}


/** Função para acionar a função load() dando um tempo para adicionar os itens */
void VideoPlayer::onLoad() {
    QTimer::singleShot(800, this, SLOT(runLoad()));
}


/** Função que possui o propósito de ajustar a ordem de execução ao remover itens da playlist anteriores ao
 * arquivo multimídia sendo reproduzido no momento */
void VideoPlayer::ajustActualItem(int item) {
    int actual = nextitem - 1;
    int c = 0;
    if (item <= actual) {
        c++;
        nextitem--;
        actualitem--;
        previousitem--;
        if (previousitem == -1)
            previousitem = playlist->setListSize() - 1;
        if (actualitem == -1)
            actualitem = playlist->setListSize() - 1;
        if (nextitem == -1)
            nextitem = playlist->setListSize() - 1;
        if (item < actual)
            playlist->selectCurrent(actual - c + 1);
    } else
        playlist->selectCurrent(actual);
    listnum.clear();
}


/** Setando o item atualmente selecionado */
void VideoPlayer::setSelect(int item) {
    actualitem = item;
    if (!playing) {
        qDebug("%s(%sDEBUG%s):%s Selecionando o item %s manualmente ...\033[0m", GRE, RED, GRE, ORA,
               Utils::mediaTitle(playlist->getItems(item)).toStdString().c_str());
    }
}


/** Função geral para execução de arquivos multimídia */
void VideoPlayer::play(const QString &isplay, int index) {
    this->setWindowTitle(Utils::mediaTitle(isplay));
    if (mediaPlayer->isPlaying() || mediaPlayer->isPaused())
        mediaPlayer->stop();
    mediaPlayer->play(isplay);
    if (index > (-1))
        playlist->selectCurrent(index);
}


/** Para executar os itens recém adicionados da playlist */
void VideoPlayer::firstPlay(const QString &isplay) {
    if (!mediaPlayer->isPlaying()) {
        qDebug("%s(%sDEBUG%s):%s Reproduzindo um Arquivo Multimídia ...\033[0m", GRE, RED, GRE, ORA);
        play(isplay, 0);
        previousitem = playlist->setListSize() - 1;
        actualitem = 0;
        nextitem = 1;
    }
}


/** Executa um item selecionado da playlist */
void VideoPlayer::doubleplay(const QString &name) {
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
void VideoPlayer::nextRand() {
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
void VideoPlayer::Next() {
    if (mediaPlayer->isPlaying()) {

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
void VideoPlayer::Previous() {
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
void VideoPlayer::playPause() {
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
void VideoPlayer::setStop() {
    playing = false;
    actualitem = playlist->selectItems();
    mediaPlayer->stop();
}


/** Função para alterar o botão play/pause */
void VideoPlayer::onPaused(bool paused) {
    if (paused) {
        qDebug("%s(%sDEBUG%s):%s Pausando ...\033[0m", GRE, RED, GRE, ORA);
        if (Utils::setIconTheme(theme, "play") == nullptr)
            playBtn->setIcon(QIcon::fromTheme(Utils::defaultIcon("play")));
        else playBtn->setIcon(QIcon(Utils::setIconTheme(theme, "play")));
        playBtn->setToolTip("Play");
    } else {
        qDebug("%s(%sDEBUG%s):%s Reproduzindo ...\033[0m", GRE, RED, GRE, ORA);
        if (Utils::setIconTheme(theme, "pause") == nullptr)
            playBtn->setIcon(QIcon::fromTheme(Utils::defaultIcon("pause")));
        else playBtn->setIcon(QIcon(Utils::setIconTheme(theme, "pause")));
        playBtn->setToolTip("Pause");
    }
}


/** Função que define alguns parâmetros ao iniciar a reprodução */
void VideoPlayer::onStart() {
    logo->setVisible(false);
    slider->setDisabled(false);
    playing = true;

    if (Utils::setIconTheme(theme, "pause") == nullptr)
        playBtn->setIcon(QIcon::fromTheme(Utils::defaultIcon("pause")));
    else playBtn->setIcon(QIcon(Utils::setIconTheme(theme, "pause")));
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
    slider->setMaximum(int(mediaPlayer->mediaStopPosition() - Utils::setDifere(mUnit))/ mUnit);
    end->setText(QTime(0, 0, 0).addMSecs(int(mediaPlayer->mediaStopPosition())).toString(QString::fromLatin1("HH:mm:ss")));

    /** Correção dos itens adicionados */
    if (playlist->setListSize() == 1)
        previousitem = nextitem = 0;

    /** Desativando Bloqueio de tela */
    if (!isblock) {
        ScreenSaver::instance().disable();
        isblock = true;
    }

    updateSlider(mediaPlayer->position());
}


/** Função que define alguns parâmetros ao parar a reprodução */
void VideoPlayer::onStop() {
    if (!playing) {
        qDebug("%s(%sDEBUG%s):%s Finalizando a Reprodução ...\033[0m", GRE, RED, GRE, ORA);
        this->setWindowTitle(QString(PRG_NAME));

        if (Utils::setIconTheme(theme, "play") == nullptr)
            playBtn->setIcon(QIcon::fromTheme(Utils::defaultIcon("play")));
        else playBtn->setIcon(QIcon(Utils::setIconTheme(theme, "play")));
        playBtn->setToolTip("Play");

        slider->setMaximum(0);
        slider->setDisabled(true);
        logo->setVisible(true);
        current->setText(QString::fromLatin1("-- -- : -- --"));
        end->setText(QString::fromLatin1("-- -- : -- --"));
        onTimeSliderLeave();

        /** Reativando Bloqueio de tela */
        if (isblock) {
            ScreenSaver::instance().enable();
            isblock = false;
        }
    }
}


/** Ativação do modo repetição */
void VideoPlayer::setReplay() {
    if (!restart) {
        restart = true;
        if (Utils::setIconTheme(theme, "replay-on") == nullptr)
            replayBtn->setIcon(QIcon::fromTheme(Utils::defaultIcon("replay-on")));
        else replayBtn->setIcon(QIcon(Utils::setIconTheme(theme, "replay-on")));
    } else {
        restart = false;
        if (Utils::setIconTheme(theme, "replay") == nullptr)
            replayBtn->setIcon(QIcon::fromTheme(Utils::defaultIcon("replay")));
        else replayBtn->setIcon(QIcon(Utils::setIconTheme(theme, "replay")));
    }
}


/** Ativação do modo aleatório */
void VideoPlayer::setShuffle() {
    if (!randplay) {
        randplay = true;
        if (Utils::setIconTheme(theme, "shuffle-on") == nullptr)
            shuffleBtn->setIcon(QIcon::fromTheme(Utils::defaultIcon("shuffle-on")));
        else shuffleBtn->setIcon(QIcon(Utils::setIconTheme(theme, "shuffle-on")));
    } else {
        randplay = false;
        if (Utils::setIconTheme(theme, "shuffle") == nullptr)
            shuffleBtn->setIcon(QIcon::fromTheme(Utils::defaultIcon("shuffle")));
        else shuffleBtn->setIcon(QIcon(Utils::setIconTheme(theme, "shuffle")));
    }
}


/** Função que mapeia um único clique e executa as ações de pausar e executar */
void VideoPlayer::detectClick() {
    if (count == 1 && !enterpos && playing && pausing)
        playPause();
    count = 0;
    pausing = false;
}


/** Gerenciar tela cheia */
void VideoPlayer::changeFullScreen() {
    if (this->isFullScreen())
        leaveFullScreen();
    else
        enterFullScreen();
}


/** Entrar no modo tela cheia */
void VideoPlayer::enterFullScreen() {
    qDebug("%s(%sDEBUG%s):%s Entrando no Modo Tela Cheia ...\033[0m", GRE, RED, GRE, ORA);

    if (this->isMaximized()) /** Mapear interface maximizada */
        maximize = true;

    this->showFullScreen();
    wctl->setVisible(false);
    moving = enterpos = false;
}


/** Sair do modo tela cheia */
void VideoPlayer::leaveFullScreen() {
    qDebug("%s(%sDEBUG%s):%s Saindo do Modo Tela Cheia ...\033[0m", GRE, RED, GRE, ORA);
    this->showNormal();

    /**
     * Precisa caso a interface já estava maximizada antes. Note que showMaximized() só funciona após a execução
     * de showNormal(), a tela não pula direto de fulscreen para maximizado.
     */
    if (maximize)
        this->showMaximized();

    wctl->setVisible(false);
    moving = enterpos = maximize = false;
}


/** Função auxiliar para minimizar os controles e a playlist */
void VideoPlayer::setHide() {
    wctl->setVisible(false);
    hideTrue();
}


/** Função para abrir as configurações */
void VideoPlayer::setSettings() {
    showsett = true;
    if (!sett)
        sett = new Settings(this);
    connect(sett, SIGNAL(emitclose()), this, SLOT(closeSettings()));
    sett->show();
}


/** Função que fecha as configurações ao receber a emissão */
void VideoPlayer::closeSettings() {
    showsett = false;
    sett->close();
}


/** Função para exibir o sobre */
void VideoPlayer::setAbout() {
    qDebug("%s(%sDEBUG%s):%s Iniciando o diálogo sobre ...\033[0m", GRE, RED, GRE, CYA);
    about->setVisible(true);
}


/** Ativar ocultação  */
void VideoPlayer::hideTrue() {
    if (enterpos) {
        qDebug("%s(%sDEBUG%s):%s Ocultação liberada ...\033[0m", GRE, RED, GRE, SHW);
        enterpos = false;
    }
}


/** Desativar ocultação */
void VideoPlayer::hideFalse() {
    if (!enterpos) {
        qDebug("%s(%sDEBUG%s):%s Ocultação impedida ...\033[0m", GRE, RED, GRE, HID);
        enterpos = true;
    }
}


/** Pré-visualização ao posicionar o mouse no slider */
void VideoPlayer::onTimeSliderHover(int pos, int value) {
    int fixH = 72;
    QPoint gpos = slider->pos();

    /** Definição da posição na tela e exibição do tooltip */
    QPoint gpos1 = mapToGlobal(gpos + QPoint(pos + 12, 5));
    QPoint gpos2 = mapToGlobal(gpos + QPoint(pos, -7));
    QToolTip::showText(gpos1, QTime(0, 0, 0).addMSecs(value * mUnit).toString(QString::fromLatin1("HH:mm:ss")), this);

    /** Arquivos de áudio não são vídeos */
    if (mediaPlayer->currentVideoStream() == (-1)) return;
    int setX = Utils::calcX(fixH, Width.toInt(), Height.toInt());

    /** Exibição da pré-visualização */
    if (!preview)
        preview = new VideoPreviewWidget(this);
    preview->setFile(mediaPlayer->file());
    preview->setTimestamp(value * mUnit);
    preview->preview();
    preview->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    preview->resize(setX, fixH);
    preview->move(gpos2 - QPoint(setX/2, fixH));
    preview->show();
}


/** Apenas para exibição do debug */
void VideoPlayer::onTimeSliderEnter() const {
    if (playing) {
        qDebug("%s(%sDEBUG%s):%s Exibindo a pré-visualização ...\033[0m", GRE, RED, GRE, CYA);
    }
}


/** Saindo da pré-visualização */
void VideoPlayer::onTimeSliderLeave() {
    if (!preview) return;
    if (preview->isVisible()) {
        qDebug("%s(%sDEBUG%s):%s Finalizando a pré-visualização ...\033[0m", GRE, RED, GRE, CYA);
        preview->close();
    }
}


/** Altera o tempo de execução ao pressionar ou mover a barra de progresso de execução */
void VideoPlayer::seekBySlider(int value) {
    if (!mediaPlayer->isPlaying()) return;
    mediaPlayer->setSeekType(QtAV::AccurateSeek);
    mediaPlayer->seek(qint64(qint64(value) * mUnit));
}


/** Altera o tempo de execução ao pressionar a barra de progresso de execução */
void VideoPlayer::seekBySlider() {
    seekBySlider(slider->value());
}


/** Função para atualizar a barra de progresso de execução */
void VideoPlayer::updateSlider(qint64 value) {
    if (mediaPlayer->isSeekable())
        slider->setValue(int(value / mUnit));
    current->setText(QTime(0, 0, 0).addMSecs(int(value)).toString(QString::fromLatin1("HH:mm:ss")));

    /** Próxima mídia */
    if (int(value) > mediaPlayer->duration() - Utils::setDifere(mUnit)) {
        if (actualitem == playlist->setListSize() - 1 && !restart && !randplay) {
            mediaPlayer->stop();
            playing = false;
            playlist->selectClean();
            previousitem = playlist->setListSize() - 1;
            actualitem = 0;
            nextitem = 1;
        } else
            Next();
    }
}


/** Função para atualizar a barra de progresso de execução */
void VideoPlayer::updateSliderUnit() {
    mUnit = mediaPlayer->notifyInterval();
    slider->setMaximum(int(mediaPlayer->mediaStopPosition() - Utils::setDifere(mUnit))/ mUnit);
    updateSlider(mediaPlayer->position());
}


/**********************************************************************************************************************/


/** Mapeador de eventos para mapear o posicionamento do mouse */
bool VideoPlayer::event(QEvent *event) {
    if (int(event->type()) == 5 && !moving && !about->isVisible()) {
        qDebug("%s(%sDEBUG%s):%s Mouse com Movimentação ...\033[0m", GRE, RED, GRE, DGR);
        wctl->setVisible(true);
        moving = true;
        Utils::arrowMouse();
    } else if (int(event->type()) == 110 && moving) {
        qDebug("%s(%sDEBUG%s):%s Mouse sem Movimentação ...\033[0m", GRE, RED, GRE, YEL);
        if (!enterpos) {
            wctl->setVisible(false);
            Utils::blankMouse();
        }
        moving = false;
    }
    return QWidget::event(event);
}


/** Mapeador para executar ações ao pressionar o mouse */
void VideoPlayer::mousePressEvent(QMouseEvent *event) {
    pausing = true;
    QWidget::mousePressEvent(event);
}


/** Mapeador para executar ações com um clique do mouse */
void VideoPlayer::mouseReleaseEvent(QMouseEvent *event) {
    if (about->isVisible()) {
        qDebug("%s(%sDEBUG%s):%s Fechando o diálogo sobre ...\033[0m", GRE, RED, GRE, CYA);
        about->setVisible(false);
        Utils::blankMouse();
        return QWidget::mouseReleaseEvent(event);
    }
    if (!click->isActive())
        click->start();
    count = count + 1; /** Contador de cliques */
    QWidget::mouseReleaseEvent(event);
}


/** Mapeador para executar ações com um duplo clique */
void VideoPlayer::mouseDoubleClickEvent(QMouseEvent *event) {
    changeFullScreen();
    pausing = false;
    QWidget::mouseDoubleClickEvent(event);
}


/** Mapeador para executar ações quando o ponteiro do mouse se encontra dentro da interface */
void VideoPlayer::enterEvent(QEvent *event) {
    if (contextmenu) {
        qDebug("%s(%sDEBUG%s):%s Finalizando o Menu de Contexto ...\033[0m", GRE, RED, GRE, CYA);
        if (!about->isVisible())
            Utils::blankMouse();
        if (showsett)
            Utils::arrowMouse();
        contextmenu = moving = false;
        wctl->setVisible(false);
    } else {
        qDebug("%s(%sDEBUG%s):%s Ponteito do Mouse Sobre a Interface ...\033[0m", GRE, RED, GRE, DGR);
        if (!about->isVisible())
            wctl->setVisible(true);
    }
    QWidget::enterEvent(event);
}


/** Mapeador para executar ações quando o ponteiro do mouse se encontra fora da interface */
void VideoPlayer::leaveEvent(QEvent *event) {
    if (!contextmenu)
        qDebug("%s(%sDEBUG%s):%s Ponteito do Mouse Fora da Interface ...\033[0m", GRE, RED, GRE, YEL);
    wctl->setVisible(false);
    enterpos = false;
    QWidget::leaveEvent(event);
}


/** Ação ao fechar o programa */
void VideoPlayer::closeEvent(QCloseEvent *event) {
    if (isblock) ScreenSaver::instance().enable();
    playlist->save();
    qDebug("%s(%sDEBUG%s):%s Finalizando o Reprodutor Multimídia !\033[0m", GRE, RED, GRE, CYA);
    event->accept();
}


/**********************************************************************************************************************/


/** Função para o menu de contexto do programa */
void VideoPlayer::ShowContextMenu(const QPoint &pos) {
    if (about->isVisible()) {
        qDebug("%s(%sDEBUG%s):%s Fechando o diálogo sobre ...\033[0m", GRE, RED, GRE, CYA);
        about->setVisible(false);
        return;
    } /** Para fechar a interface sobre */

    contextmenu = true;
    qDebug("%s(%sDEBUG%s):%s Iniciando o Menu de Contexto ...\033[0m", GRE, RED, GRE, CYA);

    auto *effect = new QGraphicsOpacityEffect();
    effect->setOpacity(OPACY);

    QMenu contextMenu(tr("Context menu"), this);
    contextMenu.setGraphicsEffect(effect);
    contextMenu.setStyleSheet(Utils::setStyle("contextmenu"));


    /** Menu de abrir */
    QAction open("Open Files", this);
    open.setShortcut(QKeySequence(CTRL | Qt::Key_O));
    if (Utils::setIconTheme(theme, "folder") == nullptr)
        open.setIcon(QIcon(style()->standardIcon(QStyle::SP_DirOpenIcon)));
    else open.setIcon(QIcon(Utils::setIconTheme(theme, "folder")));


    /** Menu tela cheia */
    QAction fullscreen("Show Fullscreen", this);
    if (this->isFullScreen())
        fullscreen.setText("Exit Fullscreen");
    fullscreen.setShortcut(QKeySequence(ALT | Qt::Key_Enter));
    if (Utils::setIconTheme(theme, "fullscreen") == nullptr)
        fullscreen.setIcon(QIcon::fromTheme("view-fullscreen"));
    else fullscreen.setIcon(QIcon(Utils::setIconTheme(theme, "fullscreen")));


    /** Menu aleatório */
    QAction shuffle("Shuffle", this);
    shuffle.setShortcut(QKeySequence(CTRL | Qt::Key_H));
    if (Utils::setIconTheme(theme, "shuffle-menu") == nullptr)
        shuffle.setIcon(QIcon::fromTheme("media-playlist-shuffle"));
    else shuffle.setIcon(QIcon(Utils::setIconTheme(theme, "shuffle-menu")));


    /** Menu repetir */
    QAction replay("Replay", this);
    replay.setShortcut(QKeySequence(CTRL | Qt::Key_T));
    if (Utils::setIconTheme(theme, "replay-menu") == nullptr)
        replay.setIcon(QIcon::fromTheme("media-playlist-repeat"));
    else replay.setIcon(QIcon(Utils::setIconTheme(theme, "replay-menu")));


    /** Menu de configuração */
    QAction settings("Settings", this);
    settings.setShortcut(QKeySequence(ALT | Qt::Key_S));
    if (Utils::setIconTheme(theme, "settings") == nullptr)
        settings.setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogApplyButton)));
    else settings.setIcon(QIcon(Utils::setIconTheme(theme, "settings")));


    /** Menu sobre */
    QAction mabout("About", this);
    if (Utils::setIconTheme(theme, "about") == nullptr)
        mabout.setIcon(QIcon::fromTheme("help-about"));
    else mabout.setIcon(QIcon(Utils::setIconTheme(theme, "about")));


    /** Ações do menu */
    connect(&open, SIGNAL(triggered()), SLOT(openMedia()));
    connect(&fullscreen, SIGNAL(triggered()), SLOT(changeFullScreen()));
    connect(&shuffle, SIGNAL(triggered()), SLOT(setShuffle()));
    connect(&replay, SIGNAL(triggered()), SLOT(setReplay()));
    connect(&settings, SIGNAL(triggered()), SLOT(setSettings()));
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

    Utils::arrowMouse();
    contextMenu.exec(mapToGlobal(pos));
}
