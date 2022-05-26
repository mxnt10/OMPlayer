#include <QDialog>
#include <QGraphicsEffect>
#include <QLayout>
#include <QTabWidget>
#include <QStyle>
#include <QWidget>

#include "Button.h"
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
    setMaximumSize(0, 0);
    setModal(false);
    theme = "circle";


    /** Opções para Renderização */
    openglwidget = new Button("radio-unselect", 24, "VideoRendererId_OpenGLWidget", "OpenGLWidget", false);
    qglwidget2 = new Button("radio-unselect", 24, "VideoRendererId_GLWidget2", "QGLWidget2", false);
    widgetrend = new Button("radio-unselect", 24, "VideoRendererId_Widget", "Widget", false);
    xvideorend = new Button("radio-unselect", 24, "VideoRendererId_XV", "XVideo", false);
    x11rend = new Button("radio-unselect", 24, "VideoRendererId_X11", "X11", false);
    connect(openglwidget, SIGNAL(pressed()), SLOT(setOpenglwidget()));
    connect(widgetrend, SIGNAL(pressed()), SLOT(setWidgetrend()));

    if (QString::compare(JsonTools::readJson("renderer"), "openglwidget") == 0) {
        if (Utils::setIconTheme(theme, "radio-select") == nullptr)
            openglwidget->setIcon(QIcon::fromTheme("emblem-checked"));
        else openglwidget->setIcon(QIcon(Utils::setIconTheme(theme, "radio-select")));
    }

    if (QString::compare(JsonTools::readJson("renderer"), "widget") == 0) {
        if (Utils::setIconTheme(theme, "radio-select") == nullptr)
            widgetrend->setIcon(QIcon::fromTheme("emblem-checked"));
        else widgetrend->setIcon(QIcon(Utils::setIconTheme(theme, "radio-select")));
    }


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


    /** Efeito para o widget principal */
    auto *effect = new QGraphicsOpacityEffect();
    effect->setOpacity(OPACY);


    /** Layout para organização das configurações */
    auto *fore = new QGridLayout();
    fore->setMargin(10);
    fore->addWidget(tab, 0, 0);
    fore->addWidget(closebtn, 1, 0, RIGHT);


    /** Widget para o plano de fundo */
    auto *found = new QWidget();
    found->setGraphicsEffect(effect);
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


/** */
void Settings::setOpenglwidget() {
    if (QString::compare(JsonTools::readJson("renderer"), "openglwidget") != 0) {
        JsonTools::writeJson("renderer", "openglwidget");
        emit emitvalue("openglwidget");

        if (Utils::setIconTheme(theme, "radio-select") == nullptr)
            openglwidget->setIcon(QIcon::fromTheme("emblem-checked"));
        else openglwidget->setIcon(QIcon(Utils::setIconTheme(theme, "radio-select")));

        if (Utils::setIconTheme(theme, "radio-unselect") == nullptr)
            widgetrend->setIcon(QIcon::fromTheme("package-available"));
        else widgetrend->setIcon(QIcon(Utils::setIconTheme(theme, "radio-unselect")));
    }
}


/** */
void Settings::setWidgetrend() {
    if (QString::compare(JsonTools::readJson("renderer"), "widget") != 0) {
        JsonTools::writeJson("renderer", "widget");
        emit emitvalue("widget");

        if (Utils::setIconTheme(theme, "radio-select") == nullptr)
            widgetrend->setIcon(QIcon::fromTheme("emblem-checked"));
        else widgetrend->setIcon(QIcon(Utils::setIconTheme(theme, "radio-select")));

        if (Utils::setIconTheme(theme, "radio-unselect") == nullptr)
            openglwidget->setIcon(QIcon::fromTheme("package-available"));
        else openglwidget->setIcon(QIcon(Utils::setIconTheme(theme, "radio-unselect")));
    }
}
