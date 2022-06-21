#include <QLayout>
#include <QTabWidget>
#include <QStyle>
#include <QtAV>
#include <QtAVWidgets>

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
    setFixedSize(200, 0);
    setModal(true);


    /** Estrutura das renderizações */
    struct {
        const char* name;
        VideoRendererId id;
        Button *btn;
        const char* desc;
    } vid_map[] = {
            { "opengl",  VideoRendererId_OpenGLWidget, openglwidget, "OpenGLWidget" },
            { "gl2",     VideoRendererId_GLWidget2,    qglwidget2,   "QGLWidget2"   },
            { "d2d",     VideoRendererId_Direct2D,     direct2d,     "Direct2D"     },
            { "gdi",     VideoRendererId_GDI,          gdi,          "GDI"          },
            { "xv",      VideoRendererId_XV,           xvideo,       "XVideo"       },
            { "x11",     VideoRendererId_X11,          x11renderer,  "X11"          },
            { "gl",      VideoRendererId_GLWidget,     qglwidget,    "QGLWidget"    },
            { "qt",      VideoRendererId_Widget,       widget,       "Widget"       }
    };


    /** Layout da renderização */
    auto *renderer = new QWidget();
    auto *oprenderer = new QGridLayout(renderer);
    VideoRenderer *vo;


    /** Selecionar renderizações existentes */
    for (int i = 0; vid_map[i].name; ++i) {
        vo = VideoRenderer::create(vid_map[i].id);
        if (vo && vo->isAvailable()) {
            vid_map[i].btn = new Button("radio-unselect", 24, NOTOOLTIP, vid_map[i].desc, false);

            if (i == 0) {
                connect(vid_map[0].btn, SIGNAL(pressed()), SLOT(setOpenglWidget()));
                openglwidget = vid_map[0].btn;
            }
            if (i == 1) {
                connect(vid_map[1].btn, SIGNAL(pressed()), SLOT(setQGLWidget2()));
                qglwidget2 = vid_map[1].btn;
            }
            if (i == 2) {
                connect(vid_map[2].btn, SIGNAL(pressed()), SLOT(setDirect2D()));
                direct2d = vid_map[2].btn;
            }
            if (i == 3) {
                connect(vid_map[3].btn, SIGNAL(pressed()), SLOT(setGDI()));
                gdi = vid_map[3].btn;
            }
            if (i == 4) {
                connect(vid_map[4].btn, SIGNAL(pressed()), SLOT(setXVideo()));
                xvideo = vid_map[4].btn;
            }
            if (i == 5) {
                connect(vid_map[5].btn, SIGNAL(pressed()), SLOT(setX11renderer()));
                x11renderer = vid_map[5].btn;
            }
            if (i == 6) {
                connect(vid_map[6].btn, SIGNAL(pressed()), SLOT(setQGLWidget()));
                qglwidget = vid_map[6].btn;
            }
            if (i == 7) {
                connect(vid_map[7].btn, SIGNAL(pressed()), SLOT(setWidget()));
                widget = vid_map[7].btn;
            }

            if (QString::compare(JsonTools::readJson("renderer"), vid_map[i].name) == 0)
                Utils::changeIcon(vid_map[i].btn, "radio-select");
            oprenderer->addWidget(vid_map[i].btn, i, 0, LEFT);
        }
    }


   /** Organização por abas */
    auto *tab = new QTabWidget();
    tab->addTab(renderer, tr("Renderer"));
    tab->setStyleSheet(Utils::setStyle("tab"));


    /** Botão para fechar a janela */
    auto *closebtn = new Button("apply", 32, tr("Apply and Close"));
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
    qDebug("%s(%sDEBUG%s):%s Fechando o diálogo de configurações ...\033[0m", GRE, RED, GRE, CYA);
    emit emitclose();
    this->close();
}


/** Resetando os botões */
void Settings::rendererSelect(Button *btn, const QString &name, const QString &desc) {
    if (openglwidget) Utils::changeIcon(openglwidget, "radio-unselect");
    if (qglwidget2) Utils::changeIcon(qglwidget2, "radio-unselect");
    if (direct2d) Utils::changeIcon(direct2d, "radio-unselect");
    if (gdi) Utils::changeIcon(gdi, "radio-unselect");
    if (xvideo) Utils::changeIcon(xvideo, "radio-unselect");
    if (x11renderer) Utils::changeIcon(x11renderer, "radio-unselect");
    if (qglwidget) Utils::changeIcon(qglwidget, "radio-unselect");
    if (widget) Utils::changeIcon(widget, "radio-unselect");

    Utils::changeIcon(btn, "radio-select");
    JsonTools::writeJson("renderer", name);
    emit emitvalue(name);
}


/** Setando a renderização OpenGLWidget */
void Settings::setOpenglWidget() {
    if (QString::compare(JsonTools::readJson("renderer"), "opengl") != 0)
        rendererSelect(openglwidget, "opengl", "OpenGLWidget");
}


/** Setando a renderização por QGLWidget2 */
void Settings::setQGLWidget2() {
    if (QString::compare(JsonTools::readJson("renderer"), "qglwidget2") != 0)
        rendererSelect(qglwidget2, "gl2", "QGLWidget2");
}


/** Setando a renderização por Direct2D */
void Settings::setDirect2D() {
    if (QString::compare(JsonTools::readJson("renderer"), "d2d") != 0)
        rendererSelect(direct2d, "d2d", "Direct2D");
}


/** Setando a renderização por GDI */
void Settings::setGDI() {
    if (QString::compare(JsonTools::readJson("renderer"), "gdi") != 0)
        rendererSelect(gdi, "gdi", "GDI");
}


/** Setando a renderização por xvideo */
void Settings::setXVideo() {
    if (QString::compare(JsonTools::readJson("renderer"), "xvideo") != 0)
        rendererSelect(xvideo, "xv", "XVideo");
}


/** Setando a renderização por x11 */
void Settings::setX11renderer() {
    if (QString::compare(JsonTools::readJson("renderer"), "x11") != 0)
        rendererSelect(x11renderer, "x11", "X11");
}


/** Setando a renderização por QGLWidget */
void Settings::setQGLWidget() {
    if (QString::compare(JsonTools::readJson("renderer"), "qglwidget") != 0)
        rendererSelect(qglwidget, "gl", "QGLWidget");
}


/** Setando a renderização por widget */
void Settings::setWidget() {
    if (QString::compare(JsonTools::readJson("renderer"), "widget") != 0)
        rendererSelect(widget, "qt", "Widget");
}
