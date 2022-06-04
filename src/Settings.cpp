#include <QLayout>
#include <QTabWidget>
#include <QStyle>
#include <QWidget>

#include "Defines.h"
#include "JsonTools.h"
#include "Settings.h"
#include "Utils.h"


/** Renderização:
 * VideoRendererId_OpenGLWidget - openglwidget
 * VideoRendererId_GLWidget2    - qglwidget2
 * VideoRendererId_Direct2D     - direct2d
 * VideoRendererId_GDI          - gdi
 * VideoRendererId_XV           - xvideo
 * VideoRendererId_X11          - x11
 * VideoRendererId_Widget       - widget
 */


/**********************************************************************************************************************/


/** Classe principal das configurações */
Settings::Settings(QWidget *parent) : QDialog(parent) {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet(Utils::setStyle("global")); // ToolTip
    setMaximumWidth(400);
    setModal(false);


    /** Opções para Renderização */
    openglwidget = new Button("radio-unselect", 24, NOTOOLTIP, "OpenGLWidget", false);
    qglwidget2 = new Button("radio-unselect", 24, NOTOOLTIP, "QGLWidget2", false);
    widgetrend = new Button("radio-unselect", 24, NOTOOLTIP, "Widget", false);
    xvideorend = new Button("radio-unselect", 24, NOTOOLTIP, "XVideo", false);
    x11rend = new Button("radio-unselect", 24, NOTOOLTIP, "X11", false);
    connect(openglwidget, SIGNAL(pressed()), SLOT(setOpenglwidget()));
    connect(qglwidget2, SIGNAL(pressed()), SLOT(setQGLWidget2()));
    connect(widgetrend, SIGNAL(pressed()), SLOT(setWidgetrend()));
    connect(xvideorend, SIGNAL(pressed()), SLOT(setXVideorend()));
    connect(x11rend, SIGNAL(pressed()), SLOT(setX11rend()));

    if (QString::compare(JsonTools::readJson("renderer"), "openglwidget") == 0)
        Utils::changeIcon(openglwidget, "radio-select");

    if (QString::compare(JsonTools::readJson("renderer"), "qglwidget2") == 0)
        Utils::changeIcon(qglwidget2, "radio-select");

    if (QString::compare(JsonTools::readJson("renderer"), "widget") == 0)
        Utils::changeIcon(widgetrend, "radio-select");

    if (QString::compare(JsonTools::readJson("renderer"), "xvideo") == 0)
        Utils::changeIcon(xvideorend, "radio-select");

    if (QString::compare(JsonTools::readJson("renderer"), "x11") == 0)
        Utils::changeIcon(x11rend, "radio-select");


    /** Layout da Renderização */
    auto *renderer = new QWidget();
    auto *oprenderer = new QGridLayout(renderer);
    oprenderer->addWidget(openglwidget, 0, 0, LEFT);
//    oprenderer->addWidget(qglwidget2, 1, 0, LEFT);
    oprenderer->addWidget(widgetrend, 2, 0, LEFT);
//    oprenderer->addWidget(xvideorend, 3, 0, LEFT);
//    oprenderer->addWidget(x11rend, 4, 0, LEFT);


   /** Organização por abas */
    auto *tab = new QTabWidget();
    tab->addTab(renderer, tr("Renderer"));
    tab->setStyleSheet(Utils::setStyle("tab"));


    /** Botão para fechar a janela */
    auto *closebtn = new Button("apply", 32, "Apply and Close");
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
}


/** Resetando os botões */
void Settings::rendererReset() {
    Utils::changeIcon(openglwidget, "radio-unselect");
    Utils::changeIcon(qglwidget2, "radio-unselect");
    Utils::changeIcon(widgetrend, "radio-unselect");
    Utils::changeIcon(xvideorend, "radio-unselect");
    Utils::changeIcon(x11rend, "radio-unselect");
}


/** Setando a renderização padrão */
void Settings::setOpenglwidget() {
    if (QString::compare(JsonTools::readJson("renderer"), "openglwidget") != 0) {
        JsonTools::writeJson("renderer", "openglwidget");
        emit emitvalue("openglwidget");
        rendererReset();
        Utils::changeIcon(openglwidget, "radio-select");
    }
}


/** Setando a renderização por qglwidget2 */
void Settings::setQGLWidget2() {
    if (QString::compare(JsonTools::readJson("renderer"), "qglwidget2") != 0) {
        JsonTools::writeJson("renderer", "qglwidget2");
        emit emitvalue("qglwidget2");
        rendererReset();
        Utils::changeIcon(qglwidget2, "radio-select");
    }
}


/** Setando a renderização por widget */
void Settings::setWidgetrend() {
    if (QString::compare(JsonTools::readJson("renderer"), "widget") != 0) {
        JsonTools::writeJson("renderer", "widget");
        emit emitvalue("widget");
        rendererReset();
        Utils::changeIcon(widgetrend, "radio-select");
    }
}


/** Setando a renderização por xvideo */
void Settings::setXVideorend() {
    if (QString::compare(JsonTools::readJson("renderer"), "xvideo") != 0) {
        JsonTools::writeJson("renderer", "xvideo");
        emit emitvalue("xvideo");
        rendererReset();
        Utils::changeIcon(xvideorend, "radio-select");
    }
}


/** Setando a renderização por x11 */
void Settings::setX11rend() {
    if (QString::compare(JsonTools::readJson("renderer"), "x11") != 0) {
        JsonTools::writeJson("renderer", "x11");
        emit emitvalue("x11");
        rendererReset();
        Utils::changeIcon(x11rend, "radio-select");
    }
}
