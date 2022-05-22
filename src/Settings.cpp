#include <QDialog>
#include <QGraphicsEffect>
#include <QLayout>
#include <QMouseEvent>
#include <QTabWidget>
#include <QWidget>

#include "Button.h"
#include "Defines.h"
#include "Settings.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Classe principal das configurações */
Settings::Settings(QWidget *parent) : QDialog(parent) {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet(Utils::setStyle("global")); // ToolTip
    setMaximumSize(0, 0);
    setModal(false);


    /** Opções para Renderização */
    openglwidget = new Button("settings", 32, nullptr, "OpenGLWidget", false);
    qglwidget2 = new Button("settings", 32, nullptr, "QGLWidget2", false);
    widgetrend = new Button("settings", 32, nullptr, "Widget", false);
    xvideorend = new Button("settings", 32, nullptr, "XVideo", false);
    x11rend = new Button("settings", 32, nullptr, "X11", false);


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
    emit emitclose();
}


/** */
void Settings::setOpenglwidget() {

}


/** */
void Settings::setWidgetrend() {

}


/**********************************************************************************************************************/


/** Evento para capturar a posição da janela */
void Settings::mousePressEvent(QMouseEvent *event) {
    setPos = event->pos();
    QWidget::mousePressEvent(event);
}


/** Evento para mover a janela */
void Settings::mouseMoveEvent(QMouseEvent *event) {
    QPoint delta = QPoint(event->pos() - setPos);
    this->move(this->x() + delta.x(), this->y() + delta.y());
    QWidget::mouseMoveEvent(event);
}
