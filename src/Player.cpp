#include <QCloseEvent>
#include <QFileDialog>
#include <QLabel>
#include <QLayout>
#include <QGraphicsEffect>
#include <QMenu>
#include <QRandomGenerator>
#include <QShortcut>
#include <QSpacerItem>
#include <QStyle>
#include <QWidget>
#include <QTimer>

#include <filesystem>
#include <MediaInfoDLL.h>

#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")

#include "Button.h"
#include "Defines.h"
#include "Player.h"
#include "PlayList.h"
#include "Slider.h"
#include "Utils.h"
#include "Widget.h"

using namespace MediaInfoDLL;
using std::filesystem::create_directory;

/**********************************************************************************************************************/


/** Construtor que define a interface do programa */
VideoPlayer::VideoPlayer(QWidget *parent) : QWidget(parent) {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[36m Iniciando o Reprodutor Multimídia ...\033[0m");
    contextmenu = enterpos = maximize = moving = playing = setinfo = false;
    restart = randplay = false;
    actualitem = nextitem = previousitem = 0;
    theme = "circle";
    mUnit = 1000;


    /** Propriedades do Programa */
    this->setWindowTitle(QString(PRG_NAME));
    this->setWindowIcon(QIcon(Utils::setIcon()));
    this->setStyleSheet(Utils::setStyle("global"));
    this->setMinimumSize(906, 510);
    this->setMouseTracking(true); /** Mapeamento do mouse */


    /** Habilitando o menu de contexto */
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(ShowContextMenu(QPoint)));


    /** Parte principal do programa que permite o funcionamento do reprodutor */
    qDebug("\033[31m------------------------------\033[34m");
    mediaPlayer = new AVPlayer(this);
    video = new VideoOutput(this);
    mediaPlayer->setRenderer(video);
    qDebug("\033[31m------------------------------\033[0m");


    /** Playlist do reprodutor */
    playlist = new PlayList(this);
    create_directory(QDir::homePath().toStdString() + "/.config/OMPlayer");
    playlist->setSaveFile(QDir::homePath() + "/.config/OMPlayer/playlist.qds");
    playlist->load();
    connect(playlist, SIGNAL(aboutToPlay(QString)), SLOT(play(QString)));
    connect(playlist, SIGNAL(firstPlay(QString)), SLOT(firstPlay(QString)));
    connect(playlist, SIGNAL(selected(int)), SLOT(setSelect(int)));
    connect(playlist, SIGNAL(emithide()), SLOT(setHide()));
    connect(playlist, SIGNAL(emitnohide()), SLOT(hideFalse()));


    /** Barra de progresso de execução */
    slider = new Slider();
    slider->setDisabled(true);
    slider->setMaximum(0);
    connect(slider, SIGNAL(sliderMoved(int)), SLOT(seekBySlider(int)));
    connect(slider, SIGNAL(sliderPressed()), SLOT(seekBySlider()));
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
    connect(mediaPlayer, SIGNAL(notifyIntervalChanged()), SLOT(updateSliderUnit()));
    connect(mediaPlayer, SIGNAL(started()), SLOT(onStart()));
    connect(mediaPlayer, SIGNAL(stopped()), SLOT(onStop()));
    connect(mediaPlayer, SIGNAL(paused(bool)), SLOT(onPaused(bool)));


    /** Botões para os controles de reprodução */
    playBtn = new Button("play", 48);
    stopBtn = new Button("stop", 32);
    nextBtn = new Button("next", 32);
    previousBtn = new Button("previous", 32);
    replayBtn = new Button("replay", 32);
    shuffleBtn = new Button("shuffle", 32);
    connect(playBtn, CLICKED, SLOT(playPause()));
    connect(stopBtn, CLICKED, SLOT(setStop()));
    connect(nextBtn, CLICKED, SLOT(Next()));
    connect(previousBtn, CLICKED, SLOT(Previous()));
    connect(replayBtn, CLICKED, SLOT(setReplay()));
    connect(shuffleBtn, CLICKED, SLOT(setShuffle()));


    /** Emissões personalizadas */
    connect(playBtn, EMITENTER, SLOT(hideFalse()));
    connect(stopBtn, EMITENTER, SLOT(hideFalse()));
    connect(nextBtn, EMITENTER, SLOT(hideFalse()));
    connect(previousBtn, EMITENTER, SLOT(hideFalse()));
    connect(replayBtn, EMITENTER, SLOT(hideFalse()));
    connect(shuffleBtn, EMITENTER, SLOT(hideFalse()));
    connect(slider, EMITENTER, SLOT(hideFalse()));
    connect(slider, EMITLEAVE, SLOT(hideTrue()));


    /** Teclas de atalho */
    auto *keyCtrlA = new QShortcut(this);
    keyCtrlA->setKey(QKeySequence(CTRL | Qt::Key_A));
    connect(keyCtrlA, ACTIVATED, SLOT(openMedia()));
    auto *keyCtrlO = new QShortcut(this);
    keyCtrlO->setKey(QKeySequence(CTRL | Qt::Key_O));
    connect(keyCtrlO, ACTIVATED, SLOT(openMedia()));
    auto *keyAltEnter = new QShortcut(this);
    keyAltEnter->setKey(QKeySequence(ALT | ENTER));
    connect(keyAltEnter, ACTIVATED, SLOT(changeFullScreen()));
    auto *keyEsc = new QShortcut(this);
    keyEsc->setKey(QKeySequence(ESC));
    connect(keyEsc, ACTIVATED, SLOT(leaveFullScreen()));
    auto *keyCtrlH = new QShortcut(this);
    keyCtrlH->setKey(QKeySequence(CTRL | Qt::Key_H));
    connect(keyCtrlH, ACTIVATED, SLOT(setShuffle()));
    auto *keyCtrlT = new QShortcut(this);
    keyCtrlT->setKey(QKeySequence(CTRL | Qt::Key_T));
    connect(keyCtrlT, ACTIVATED, SLOT(setReplay()));


    /** Opacidade de 80% para os widgets de fundo dos controles e playlist */
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[36m Preparando o layout da interface ...\033[0m");
    auto *effect1 = new QGraphicsOpacityEffect();
    effect1->setOpacity(OPACY);
    auto *effect2 = new QGraphicsOpacityEffect();
    effect2->setOpacity(OPACY);


    /** Assistentes para mapear quando a ocultação dos controles não deve ser feita */
    auto *nohide1 = new Widget();
    connect(nohide1, EMITENTER, SLOT(hideFalse()));
    connect(nohide1, EMITLEAVE, SLOT(hideTrue()));
    auto *nohide2 = new Widget();
    connect(nohide2, EMITENTER, SLOT(hideFalse()));
    connect(nohide2, EMITLEAVE, SLOT(hideTrue()));
    auto *nohide3 = new Widget();
    connect(nohide3, EMITENTER, SLOT(hideFalse()));
    connect(nohide3, EMITLEAVE, SLOT(hideTrue()));


    /** Plano de fundo semitransparente da função about */
    auto *qabout = new QWidget();
    qabout->setFixedSize(856, 460);
    qabout->setGraphicsEffect(effect2);
    qabout->setStyleSheet(Utils::setStyle("widget"));


    /** Ajuste do layout do about */
    auto *vlabout = new QVBoxLayout();
    vlabout->addWidget(qabout);
    vlabout->setAlignment(CENTER);


    /** Nome do programa */
    auto *name = new QLabel();
    name->setText(PRG_DESC);
    name->setMaximumHeight(40);
    name->setAlignment(TOP);
    name->setStyleSheet("font-size: 24pt");


    /** Descrição do programa */
    auto *description = new QLabel();
    description->setText(About::getDescription());
    name->setAlignment(TOP);


    /** Definição do icon logo */
    auto *iconlogo = new QLabel();
    iconlogo->setMinimumHeight(200);
    iconlogo->setPixmap(QPixmap(Utils::setIcon()));
    iconlogo->setAlignment(CENTER);


    /** Versão do programa */
    auto *version = new QLabel();
    version->setText("Version " + QString::fromStdString(VERSION));
    version->setAlignment(RIGHT);
    version->setStyleSheet("font-size: 12pt");


    /** Demais informações */
    auto *maintainer = new QLabel();
    maintainer->setText(About::getTextMaintainer());
    maintainer->setAlignment(BOTTON);
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


    /** Ajuste dos controles */
    auto *fgctl = new QVBoxLayout();
    fgctl->setMargin(20);
    fgctl->addWidget(slider);
    fgctl->addLayout(buttons);


    /** Conteiner que permite o mapeamento do mouse, ocultação e exibição dos controles */
    wctl = new QWidget();
    wctl->setMouseTracking(true);


    /** Layout dos controles de reprodução */
    auto *controls = new QGridLayout(wctl);
    controls->setMargin(0);
    controls->addWidget(playlist, 0, 0);
    controls->addLayout(bgctl, 1, 0);
    controls->addLayout(fgctl, 1, 0);
    controls->setAlignment(BOTTON);


    /** Definição da logo */
    logo = new QLabel();
    logo->setPixmap(QPixmap(Utils::setIcon(true)));
    logo->setAlignment(CENTER);


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


    /** Temporizador para manter a tela sempre ativa */
    noscreensaver = new QTimer();
    connect(noscreensaver, SIGNAL(timeout()), SLOT(blockScreenSaver()));
}


/** Destrutor */
VideoPlayer::~VideoPlayer() {
    playlist->save();
}


/**********************************************************************************************************************/


/** Função para abrir arquivos multimídia */
void VideoPlayer::openMedia(const QStringList &parms) {
    playlist->addItems(parms);
}


/** Função anti-bloqueio de tela */
void VideoPlayer::blockScreenSaver() {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[36m Acionando anti-bloqueio ...\033[0m");
    PyRun_SimpleString("key.tap_key(key.control_key)\n");
}


/** Setando o item atualmente selecionado */
void VideoPlayer::setSelect(int item) {
    actualitem = item;
    if (!playing)
        playlist->setIndex();
}


/** Para executar os itens recém adicionados da playlist */
void VideoPlayer::firstPlay(const QString &isplay){
    if (!mediaPlayer->isPlaying()) {
        qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[34m Reproduzindo um Arquivo Multimídia ...\033[0m");
        this->setWindowTitle(Utils::mediaTitle(isplay));
        mediaPlayer->play(isplay);
        playlist->selectNext();
        previousitem = playlist->setListSize() - 1;
        actualitem = 0;
        nextitem = 1;
    }
}


/** Executa um item selecionado da playlist */
void VideoPlayer::play(const QString &name) {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[34m Reproduzindo um Arquivo Multimídia ...\033[0m");
    this->setWindowTitle(Utils::mediaTitle(name));
    PlayListItem item;
    item.setUrl(name);
    item.setTitle(Utils::mediaTitle(name));
    mediaPlayer->stop();
    setinfo = false;
    mediaPlayer->play(name);

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
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[34m Reproduzindo item aleatório ...\033[0m");

    if (listnum.size() < playlist->setListSize() - 1) {
        actualitem = QRandomGenerator::global()->bounded(playlist->setListSize() - 1);

        /**
         * No método aleatório convencional, alguns itens são reproduzidos mais vezes que os outros. Portanto,
         * criei esse método para executar os itens aleatoriamente só que um de cada vez. Assim, quando todos os
         * arquivos multimídia forem reproduzidos aleatoriamente, aí sim é zerado o contador para recameçar a
         * execução aleatória dos itens denovo.
         */

        while (!listnum.filter(QRegExp("^(" + QString::number(actualitem) + ")$")).isEmpty())
            actualitem = QRandomGenerator::global()->bounded(playlist->setListSize() - 1);

        listnum.append(QString::number(actualitem));
    } else {
        actualitem = QRandomGenerator::global()->bounded(playlist->setListSize() - 1);
        listnum.clear();
        listnum.append(QString::number(actualitem));
    }

    this->setWindowTitle(Utils::mediaTitle(playlist->getItems(actualitem)));
    mediaPlayer->stop();
    setinfo = false;
    mediaPlayer->play(playlist->getItems(actualitem));

    /** Cálculo dos próximos itens a serem executados */
    nextitem = actualitem + 1;
    previousitem = actualitem - 1;
    if (actualitem == 0)
        previousitem = playlist->setListSize() - 1;
    if (actualitem == playlist->setListSize() - 1)
        nextitem = 0;
    playlist->selectCurrent(actualitem);
}


/** Botão next */
void VideoPlayer::Next() {
    if (mediaPlayer->isPlaying()) {
        qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[34m Reproduzindo o próximo item ...\033[0m");
        this->setWindowTitle(Utils::mediaTitle(playlist->getItems(nextitem)));
        mediaPlayer->stop();
        setinfo = false;
        mediaPlayer->play(playlist->getItems(nextitem));

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
        playlist->selectNext();
    }
}


/** Botão previous */
void VideoPlayer::Previous() {
    if (mediaPlayer->isPlaying()) {
        qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[34m Reproduzindo um item anterior ...\033[0m");
        this->setWindowTitle(Utils::mediaTitle(playlist->getItems(previousitem)));
        mediaPlayer->stop();
        setinfo = false;
        mediaPlayer->play(playlist->getItems(previousitem));

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
        playlist->selectPrevious();
    }
}


/** Função para o botão de play/pause */
void VideoPlayer::playPause() {
    if (!mediaPlayer->isPlaying()) {
        if (playlist->setListSize() > 0) {
            qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[34m Reproduzindo um Arquivo Multimídia ...\033[0m");
            this->setWindowTitle(Utils::mediaTitle(playlist->getItems(actualitem)));
            setinfo = false;
            mediaPlayer->play(playlist->getItems(actualitem));
            playlist->selectPlay();
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
        qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[34m Pausado ...\033[0m");
        if (Utils::setIconTheme(theme, "play") == nullptr)
            playBtn->setIcon(QIcon::fromTheme(Utils::defaultIcon("play")));
        else playBtn->setIcon(QIcon(Utils::setIconTheme(theme, "play")));
    } else {

        qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[34m Reproduzindo ...\033[0m");
        if (Utils::setIconTheme(theme, "pause") == nullptr)
            playBtn->setIcon(QIcon::fromTheme(Utils::defaultIcon("pause")));
        else playBtn->setIcon(QIcon(Utils::setIconTheme(theme, "pause")));
    }
}


/** Função que define alguns parâmetros ao iniciar a reprodução */
void VideoPlayer::onStart() {
    Py_Initialize();
    PyRun_SimpleString("from pykeyboard import PyKeyboard\n"
                       "key = PyKeyboard()\n");
    noscreensaver->start(15 * 1000);

    if (Utils::setIconTheme(theme, "pause") == nullptr)
        playBtn->setIcon(QIcon::fromTheme(Utils::defaultIcon("pause")));
    else playBtn->setIcon(QIcon(Utils::setIconTheme(theme, "pause")));
    logo->setVisible(false);
    slider->setDisabled(false);
    playing = true;

    /** Correção dos itens adicionados */
    if (playlist->setListSize() == 1)
        previousitem = nextitem = 0;

    updateSlider(mediaPlayer->position());
}


/** Função que define alguns parâmetros ao parar a reprodução */
void VideoPlayer::onStop() {
    if (!playing) {
        qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[34m Finalizando a Reprodução ...\033[0m");
        this->setWindowTitle(QString(PRG_NAME));
        mediaPlayer->stop();

        if (Utils::setIconTheme(theme, "play") == nullptr)
            playBtn->setIcon(QIcon::fromTheme(Utils::defaultIcon("play")));
        else playBtn->setIcon(QIcon(Utils::setIconTheme(theme, "play")));

        slider->setMaximum(0);
        slider->setDisabled(true);
        logo->setVisible(true);
        noscreensaver->stop();
        Py_FinalizeEx();
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


/** Gerenciar tela cheia */
void VideoPlayer::changeFullScreen() {
    if (this->isFullScreen())
        leaveFullScreen();
    else
        enterFullScreen();
}


/** Entrar no modo tela cheia */
void VideoPlayer::enterFullScreen() {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[34m Entrando no Modo Tela Cheia ...\033[0m");

    if (this->isMaximized()) /** Mapear interface maximizada */
        maximize = true;

    this->showFullScreen();
    wctl->setVisible(false);
    moving = enterpos = false;
}


/** Sair do modo tela cheia */
void VideoPlayer::leaveFullScreen() {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[34m Saindo do Modo Tela Cheia ...\033[0m");
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


/** Exibir sobre */
void VideoPlayer::setAbout() {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[34m Diálogo sobre ...\033[0m");
    about->setVisible(true);
}


/** Ativar ocultação  */
void VideoPlayer::hideTrue() {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[32m Ocultação liberada ...\033[0m");
    if (enterpos)
        enterpos = false;
}


/** Desativar ocultação */
void VideoPlayer::hideFalse() {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[33m Ocultação impedida ...\033[0m");
    if (!enterpos)
        enterpos = true;
}


/**********************************************************************************************************************/


/** Altera o tempo de execução ao pressionar ou mover a barra de progresso de execução */
void VideoPlayer::seekBySlider(int value) {
    if (!mediaPlayer->isPlaying()) return;
    mediaPlayer->seek(qint64(qint64(value) * mUnit));
}


/** Altera o tempo de execução ao pressionar a barra de progresso de execução */
void VideoPlayer::seekBySlider() {
    seekBySlider(slider->value());
}


/**********************************************************************************************************************/


/** Função para atualizar a barra de progresso de execução */
void VideoPlayer::updateSlider(qint64 value) {
    slider->setRange(0, int(mediaPlayer->duration() / mUnit));
    slider->setValue(int(value / mUnit));

    /** Atualizar o status do item da playlist se necessário */
    if (!setinfo && playlist->setDuration() == 0) {
        int row = playlist->selectItems();
        QString url = mediaPlayer->file();
        qint64 duration = mediaPlayer->duration();

        MI.Open(url.toStdString());
        QString format = MI.Get(Stream_General, 0, "Format", Info_Text, Info_Name).c_str();
        MI.Close();

        playlist->removeSelectedItems();
        playlist->insert(url, row, duration, format);
    } else {
        setinfo = true;
    }

    /** Próxima mídia */
    if (int(value / mUnit) == mediaPlayer->duration() / mUnit - 1) {
        //QTimer::singleShot(900, this, SLOT());
        if (actualitem == playlist->setListSize() - 1 && !restart && !randplay) {
            mediaPlayer->stop();
            playing = false;
            playlist->selectClean();
            previousitem = playlist->setListSize() - 1;
            actualitem = 0;
            nextitem = 1;
        } else {
            setinfo = false;
            if (randplay)
                nextRand();
            else
                Next();
        }
    }
}


/** Função para atualizar a barra de progresso de execução */
void VideoPlayer::updateSliderUnit() {
    mUnit = mediaPlayer->notifyInterval();
    onStart();
}


/**********************************************************************************************************************/


/** Mapeador de eventos para mapear o posicionamento do mouse */
bool VideoPlayer::event(QEvent *event) {
    if (int(event->type()) == 5 && !moving && !about->isVisible()) {
        qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[32m Mouse com Movimentação ...\033[0m");
        wctl->setVisible(true);
        moving = true;
        Utils::arrowMouse();
    } else if (int(event->type()) == 110 && moving) {
        qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[33m Mouse sem Movimentação ...\033[0m");
        if (!enterpos) {
            wctl->setVisible(false);
            Utils::blankMouse();
        }
        moving = false;
    }
    return QWidget::event(event);
}


/** Mapeador para executar ações com um clique do mouse */
void VideoPlayer::mouseReleaseEvent(QMouseEvent *event) {
    if (about->isVisible()) {
        about->setVisible(false);
        Utils::blankMouse();
        return;
    }
}


/** Mapeador para executar ações com um duplo clique */
void VideoPlayer::mouseDoubleClickEvent(QMouseEvent *event) {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[34m Duplo clique com o mouse ...\033[0m");
    changeFullScreen();
}


/** Mapeador para executar ações quando o ponteiro do mouse se encontra dentro da interface */
void VideoPlayer::enterEvent(QEvent *event) {
    if (contextmenu) {
        qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[34m Finalizando o Menu de Contexto ...\033[0m");
        if (!about->isVisible())
            Utils::blankMouse();
        contextmenu = moving = false;
        wctl->setVisible(false);
    } else {
        qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[32m Ponteito do Mouse Sobre a Interface ...\033[0m");
        if (!about->isVisible())
            wctl->setVisible(true);
    }
}


/** Mapeador para executar ações quando o ponteiro do mouse se encontra fora da interface */
void VideoPlayer::leaveEvent(QEvent *event) {
    if (!contextmenu)
        qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[33m Ponteito do Mouse Fora da Interface ...\033[0m");
    wctl->setVisible(false);
    enterpos = false;
}


/** Ação ao fechar o programa */
void VideoPlayer::closeEvent(QCloseEvent *event) {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[36m Finalizando o Reprodutor Multimídia !\033[0m");
    playlist->save();
    Py_FinalizeEx();
    event->accept();
}


/**********************************************************************************************************************/


/** Função para o menu de contexto do programa */
void VideoPlayer::ShowContextMenu(const QPoint &pos) {
    if (about->isVisible()) {
        about->setVisible(false);
        return;
    } /** Para fechar a interface sobre */

    contextmenu = true;
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[34m Iniciando o Menu de Contexto ...\033[0m");

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
    connect(&open, TRIGGERED, SLOT(openMedia()));
    connect(&fullscreen, TRIGGERED, SLOT(changeFullScreen()));
    connect(&shuffle, TRIGGERED, SLOT(setShuffle()));
    connect(&replay, TRIGGERED, SLOT(setReplay()));
    connect(&mabout, TRIGGERED, SLOT(setAbout()));


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
