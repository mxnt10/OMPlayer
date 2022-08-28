#include <QComboBox>
#include <QGroupBox>
#include <QLayout>
#include <QTabWidget>

#include "Label.h"
#include "JsonTools.h"
#include "Settings.h"
#include "Utils.h"

using namespace QtAV;


/**********************************************************************************************************************/


/** Classe principal das configurações */
Settings::Settings(QWidget *parent) : QDialog(parent) {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet(Utils::setStyle("global")); // ToolTip
    setFixedSize(0, 0);
    setModal(true);


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
            vid_map[i].btn = new Button(Button::radio, "radio-unselect", 18, nullptr, vid_map[i].name);

            if (i == 0) connect(vid_map[0].btn, SIGNAL(pressed()), SLOT(setOpenglWidget()));
            if (i == 1) connect(vid_map[1].btn, SIGNAL(pressed()), SLOT(setQGLWidget2()));
            if (i == 2) connect(vid_map[2].btn, SIGNAL(pressed()), SLOT(setDirect2D()));
            if (i == 3) connect(vid_map[3].btn, SIGNAL(pressed()), SLOT(setGDI()));
            if (i == 4) connect(vid_map[4].btn, SIGNAL(pressed()), SLOT(setXVideo()));
            if (i == 5) connect(vid_map[5].btn, SIGNAL(pressed()), SLOT(setX11renderer()));
            if (i == 6) connect(vid_map[6].btn, SIGNAL(pressed()), SLOT(setQGLWidget()));
            if (i == 7) connect(vid_map[7].btn, SIGNAL(pressed()), SLOT(setWidget()));

            if (QString::compare(JsonTools::stringJson("renderer"), vid_map[i].name) == 0)
                Utils::changeIcon(vid_map[i].btn, "radio-select");
            oprenderer->addWidget(vid_map[i].btn, i, 0, LEFT);
        }
    }


    /** Configurações de tema */
    auto *combotheme = new QComboBox();
    combotheme->addItems(Utils::subdirIcons());
    combotheme->setCurrentText(JsonTools::stringJson("theme"));
    connect(combotheme, SIGNAL(currentTextChanged(QString)), SLOT(setIcon(QString)));

    auto *labeltheme = new Label(RIGHT, 0, nullptr, tr("Icon Themes") + ": ");
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
    tab->addTab(renderer, tr("Renderer"));
    tab->setStyleSheet(Utils::setStyle("tab"));


    /** Botão para fechar a janela */
    closebtn = new Button(Button::button, "apply", 32, tr("Apply and Close"));
    connect(closebtn, SIGNAL(pressed()), SLOT(onClose()));


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
    qDebug("%s(%sSettings%s):%s Fechando o diálogo de configurações ...\033[0m", GRE, RED, GRE, CYA);
    emit emitclose();
    this->close();
}


/** Resetando os botões */
void Settings::rendererSelect(Button *btn, const QString &name) {
    qDebug("%s(%sSettings%s):%s Selecionando renderizador %s ...\033[0m", GRE, RED, GRE, ORA, qUtf8Printable(name));
    for (int i = 0; i < 8; ++i)
        if (vid_map[i].btn) Utils::changeIcon(vid_map[i].btn, "radio-unselect");

    Utils::changeIcon(btn, "radio-select");
    JsonTools::stringJson("renderer", name);
    emit emitvalue(name);
}


/** Setando a renderização OpenGLWidget */
void Settings::setOpenglWidget() {
    if (QString::compare(JsonTools::stringJson("renderer"), vid_map[0].name) != 0)
        rendererSelect(vid_map[0].btn, vid_map[0].name);
}


/** Setando a renderização por QGLWidget2 */
void Settings::setQGLWidget2() {
    if (QString::compare(JsonTools::stringJson("renderer"), vid_map[1].name) != 0)
        rendererSelect(vid_map[1].btn, vid_map[1].name);
}


/** Setando a renderização por Direct2D */
void Settings::setDirect2D() {
    if (QString::compare(JsonTools::stringJson("renderer"), vid_map[2].name) != 0)
        rendererSelect(vid_map[2].btn, vid_map[2].name);
}


/** Setando a renderização por GDI */
void Settings::setGDI() {
    if (QString::compare(JsonTools::stringJson("renderer"), vid_map[3].name) != 0)
        rendererSelect(vid_map[3].btn, vid_map[3].name);
}


/** Setando a renderização por xvideo */
void Settings::setXVideo() {
    if (QString::compare(JsonTools::stringJson("renderer"), vid_map[4].name) != 0)
        rendererSelect(vid_map[4].btn, vid_map[4].name);
}


/** Setando a renderização por x11 */
void Settings::setX11renderer() {
    if (QString::compare(JsonTools::stringJson("renderer"), vid_map[5].name) != 0)
        rendererSelect(vid_map[5].btn, vid_map[5].name);
}


/** Setando a renderização por QGLWidget */
void Settings::setQGLWidget() {
    if (QString::compare(JsonTools::stringJson("renderer"), vid_map[6].name) != 0)
        rendererSelect(vid_map[6].btn, vid_map[6].name);
}


/** Setando a renderização por widget */
void Settings::setWidget() {
    if (QString::compare(JsonTools::stringJson("renderer"), vid_map[7].name) != 0)
        rendererSelect(vid_map[7].btn, vid_map[7].name);
}


/** Modificando as configurações de temas */
void Settings::setIcon(const QString &index) {
    qDebug("%s(%sSettings%s):%s Tema de ícones %s ...\033[0m", GRE, RED, GRE, ORA, qUtf8Printable(index));
    JsonTools::stringJson("theme", index);
    Utils::initUtils(Utils::Theme);

    for (int i = 0; i < 8; ++i) {
        if (vid_map[i].btn) {
            if (QString::compare(JsonTools::stringJson("renderer"), vid_map[i].name) == 0)
                Utils::changeIcon(vid_map[i].btn, "radio-select");
            else Utils::changeIcon(vid_map[i].btn, "radio-unselect");
        }
    }

    Utils::changeIcon(closebtn, "apply");
    emit changethemeicon();
}
