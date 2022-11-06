#include <QComboBox>
#include <QGroupBox>
#include <QLayout>
#include <QTabWidget>
#include <Label>
#include <Utils>

#include "Settings.h"

using namespace QtAV;


/**********************************************************************************************************************/


/** Classe principal das configurações */
Settings::Settings(QWidget *parent) : QDialog(parent) {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setFixedWidth(400);
    setModal(true);
    setFocus();


    /** Opções de decodificação */
    decoder = new Decoder();


    /** Estrutura das renderizações */
    vid_map = new Render[8];
    vid_map[0] = {"OpenGL",     VideoRendererId_OpenGLWidget, opengl      };
    vid_map[1] = {"QGLWidget2", VideoRendererId_GLWidget2,    qglwidget2  };
    vid_map[2] = {"Direct2D",   VideoRendererId_Direct2D,     direct2d    };
    vid_map[3] = {"GDI",        VideoRendererId_GDI,          gdi         };
    vid_map[4] = {"XVideo",     VideoRendererId_XV,           xvideo      };
    vid_map[5] = {"X11",        VideoRendererId_X11,          x11renderer };
    vid_map[6] = {"QGLWidget",  VideoRendererId_GLWidget,     qglwidget   };
    vid_map[7] = {"Widget",     VideoRendererId_Widget,       widget      };


    /** Layout da renderização */
    auto *renderer = new QWidget();
    auto *oprenderer = new QGridLayout(renderer);


    /** Selecionar renderizações existentes */
    for (int i = 0; i < 8; ++i) {
        vo = VideoRenderer::create(vid_map[i].id);
        if (vo && vo->isAvailable()) {
            vid_map[i].btn = new QRadioButton(vid_map[i].name);
            vid_map[i].btn->setFocusPolicy(Qt::NoFocus);

            if (i == 0) connect(vid_map[0].btn, &QRadioButton::toggled, [this](){ rendererSelect(vid_map[0]); });
            else if (i == 1) connect(vid_map[1].btn, &QRadioButton::toggled, [this](){ rendererSelect(vid_map[1]); });
            else if (i == 2) connect(vid_map[2].btn, &QRadioButton::toggled, [this](){ rendererSelect(vid_map[2]); });
            else if (i == 3) connect(vid_map[3].btn, &QRadioButton::toggled, [this](){ rendererSelect(vid_map[3]); });
            else if (i == 4) connect(vid_map[4].btn, &QRadioButton::toggled, [this](){ rendererSelect(vid_map[4]); });
            else if (i == 5) connect(vid_map[5].btn, &QRadioButton::toggled, [this](){ rendererSelect(vid_map[5]); });
            else if (i == 6) connect(vid_map[6].btn, &QRadioButton::toggled, [this](){ rendererSelect(vid_map[6]); });
            else if (i == 7) connect(vid_map[7].btn, &QRadioButton::toggled, [this](){ rendererSelect(vid_map[7]); });

            if (QString::compare(JsonTools::stringJson("renderer"), vid_map[i].name) == 0)
                vid_map[i].btn->setChecked(true);
            oprenderer->addWidget(vid_map[i].btn, i, 0, LEFT);
        }
    }


    /** Configurações de tema */
    auto *combotheme = new QComboBox();
    combotheme->addItems(Utils::subdirIcons());
    combotheme->setCurrentText(JsonTools::stringJson("theme"));
    connect(combotheme, &QComboBox::currentTextChanged, this, &Settings::setIcon);

    auto *labeltheme = new Label(RIGHT, 0, tr("Icon Themes") + ": ");
    auto *themes = new QGridLayout();
    themes->addWidget(labeltheme, 0, 0);
    themes->addWidget(combotheme, 0, 1);


    /** Definição dos grupos */
    auto *themebox = new QGroupBox(tr("Themes"));
    themebox->setLayout(themes);


    /** Itens gerais */
    auto *general = new QWidget();
    auto *opgeneral = new QGridLayout(general);
    opgeneral->addWidget(themebox);


   /** Organização por abas */
    auto *tab = new QTabWidget();
    tab->addTab(general, tr("General"));
    tab->addTab(decoder, tr("Decoder"));
    tab->addTab(renderer, tr("Renderer"));

    QString str = Utils::setStyle("tab").replace(
            QRegExp("_RADIO_UNCHECKED__"),
            Utils::setIconTheme(Utils::setThemeName(),"radio-unselect")).replace(
                    QRegExp("_RADIO_UNCHECKED_HOVER_"),
                    Utils::setIconTheme(Utils::setThemeName(),"radio-hover")).replace(
                            QRegExp("_RADIO_CHECKED_"),
                            Utils::setIconTheme(Utils::setThemeName(),"radio-select"));
    tab->setStyleSheet(str);


    /** Botão para fechar a janela */
    closebtn = new Button(Button::Default, "apply", 32);
    connect(closebtn, &Button::pressed, this, &Settings::onClose);


    /** Layout para organização das configurações */
    auto *fore = new QGridLayout();
    fore->setMargin(10);
    fore->addWidget(tab, 0, 0);
    fore->addWidget(closebtn, 1, 0, RIGHT);


    /** Widget para o plano de fundo */
    auto *found = new QWidget();
    found->setStyleSheet(Utils::setStyle("widget"));


    /** Layout principal */
    auto *layout = new QGridLayout();
    layout->setMargin(0);
    layout->addWidget(found, 0, 0);
    layout->addLayout(fore, 0, 0);
    setLayout(layout);
}


/** Destrutor */
Settings::~Settings() = default;


/**********************************************************************************************************************/


/** Emissão para fechar a janela */
void Settings::onClose() {
    qDebug("%s(%sSettings%s)%s::%sFechando o diálogo de configurações ...\033[0m", GRE, RED, GRE, RED, CYA);
    Q_EMIT emitclose();
    this->close();
}


/** Resetando os botões */
void Settings::rendererSelect(Render &value) {
    if (QString::compare(JsonTools::stringJson("renderer"), value.name) == 0) return;
    qDebug("%s(%sSettings%s)%s::%sSelecionando renderizador %s ...\033[0m", GRE, RED, GRE, RED, ORA, STR(value.name));

    JsonTools::stringJson("renderer", value.name);
    Q_EMIT emitvalue(value.name);
}


/** Modificando as configurações de temas */
void Settings::setIcon(const QString &index) {
    qDebug("%s(%sSettings%s)%s::%sTema de ícones %s ...\033[0m", GRE, RED, GRE, RED, ORA, STR(index));
    JsonTools::stringJson("theme", index);

    Utils::initUtils(Utils::Theme);
    Utils::changeIcon(closebtn, "apply");

    Q_EMIT changethemeicon();
}


/** Retorna as opções de decodificação */
QVariantHash Settings::videoDecoderOptions() const {
    return decoder->videoDecoderOptions();
}


/** Retorna a prioridade de execução */
QVector<QtAV::VideoDecoderId> Settings::decoderPriorityNames() {
    return decoder->decoderPriorityNames();
}


