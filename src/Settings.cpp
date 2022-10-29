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
    setFixedSize(0, 0);
    setModal(true);
    setFocus();


    /** layouts */
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
            vid_map[i].btn = new Button(Button::Radio, "radio-unselect", 18, vid_map[i].name);

            if (i == 0) connect(vid_map[0].btn, &Button::pressed, [this](){ rendererSelect(vid_map[0]); });
            if (i == 1) connect(vid_map[1].btn, &Button::pressed, [this](){ rendererSelect(vid_map[1]); });
            if (i == 2) connect(vid_map[2].btn, &Button::pressed, [this](){ rendererSelect(vid_map[2]); });
            if (i == 3) connect(vid_map[3].btn, &Button::pressed, [this](){ rendererSelect(vid_map[3]); });
            if (i == 4) connect(vid_map[4].btn, &Button::pressed, [this](){ rendererSelect(vid_map[4]); });
            if (i == 5) connect(vid_map[5].btn, &Button::pressed, [this](){ rendererSelect(vid_map[5]); });
            if (i == 6) connect(vid_map[6].btn, &Button::pressed, [this](){ rendererSelect(vid_map[6]); });
            if (i == 7) connect(vid_map[7].btn, &Button::pressed, [this](){ rendererSelect(vid_map[7]); });

            if (QString::compare(JsonTools::stringJson("renderer"), vid_map[i].name) == 0)
                Utils::changeIcon(vid_map[i].btn, "radio-select");
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
    tab->setStyleSheet(Utils::setStyle("tab"));


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
    emit emitclose();
    this->close();
}


/** Resetando os botões */
void Settings::rendererSelect(Render &value) {
    if (QString::compare(JsonTools::stringJson("renderer"), value.name) == 0) return;

    qDebug("%s(%sSettings%s)%s::%sSelecionando renderizador %s ...\033[0m", GRE, RED, GRE, RED, ORA, STR(value.name));

    for (int i = 0; i < 8; ++i)
        if (vid_map[i].btn) Utils::changeIcon(vid_map[i].btn, "radio-unselect");

    Utils::changeIcon(value.btn, "radio-select");
    JsonTools::stringJson("renderer", value.name);
    emit emitvalue(value.name);
}


/** Modificando as configurações de temas */
void Settings::setIcon(const QString &index) {
    qDebug("%s(%sSettings%s)%s::%sTema de ícones %s ...\033[0m", GRE, RED, GRE, RED, ORA, STR(index));
    JsonTools::stringJson("theme", index);
    Utils::initUtils(Utils::Theme);

    for (int i = 0; i < 8; ++i)
        if (vid_map[i].btn) {
            if (QString::compare(JsonTools::stringJson("renderer"), vid_map[i].name) == 0)
                Utils::changeIcon(vid_map[i].btn, "radio-select");
            else Utils::changeIcon(vid_map[i].btn, "radio-unselect");
        }

    Utils::changeIcon(closebtn, "apply");
    emit changethemeicon();
}
