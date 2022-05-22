#include <QGraphicsEffect>
#include <QGridLayout>
#include <QMouseEvent>

#include "About.h"
#include "Button.h"
#include "Defines.h"
#include "Label.h"
#include "Utils.h"


/**********************************************************************************************************************/


About::About(QWidget *parent) : QDialog(parent) {
    qDebug("%s(%sDEBUG%s):%s Iniciando o diálogo sobre ...\033[0m", GRE, RED, GRE, CYA);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet(Utils::setStyle("global")); // ToolTip
    setMaximumSize(0, 0);
    setModal(false);


    /** Nome do programa e descrição */
    auto *description = new Label(TOP, 0, nullptr, getDescription());
    auto *name = new Label(TOP, 0, nullptr, PRG_DESC);
    name->setStyleSheet("font-size: 24pt");


    /** Definição do icon logo */
    auto *iconlogo = new Label(CENTER);
    iconlogo->setPixmap(QPixmap(Utils::setIcon()));


    /** Versão do programa e demais informações*/
    auto *version = new Label(static_cast<const QFlag>(RIGHT | CENTER), 0, nullptr, "Version " + QString::fromStdString(VERSION));
    auto *maintainer = new Label(BOTTON, 0, nullptr, getTextMaintainer());
    version->setStyleSheet("font-size: 11pt");
    maintainer->setStyleSheet("font-size: 11pt");


    /** Botão para fechar a janela */
    auto *closebtn = new Button("apply", 32, "Apply and Close");
    connect(closebtn, SIGNAL(pressed()), SLOT(onClose()));


    /** Organizaçãodas informações do Sobre */
    auto *labout = new QGridLayout();
    labout->setMargin(20);
    labout->addWidget(name, 0, 0);
    labout->addWidget(version, 0, 1);
    labout->addWidget(description, 1, 0, 1, 2);
    labout->addWidget(iconlogo, 2, 0, 1, 2);
    labout->addWidget(maintainer, 3, 0);
    labout->addWidget(closebtn, 3, 1, RIGHT | BOTTON);


    /** Efeito para o widget principal */
    auto *effect = new QGraphicsOpacityEffect();
    effect->setOpacity(OPACY);


    /** Widget para o plano de fundo */
    auto *found = new QWidget();
    found->setGraphicsEffect(effect);
    found->setStyleSheet(Utils::setStyle("widget"));


    /** Layout principal */
    auto *layout = new QGridLayout();
    layout->setMargin(0);
    layout->addWidget(found, 0, 0);
    layout->addLayout(labout, 0, 0);
    setLayout(layout);
}


/** Destrutor */
About::~About() = default;


/**********************************************************************************************************************/


/** Emissão para fechar a janela */
void About::onClose() {
    qDebug("%s(%sDEBUG%s):%s Fechando o diálogo sobre ...\033[0m", GRE, RED, GRE, CYA);
    emit emitclose();
}


/** Informações adicionais */
QString About::getTextMaintainer() {
    return tr("Maintainer") + ": " + QString::fromStdString(MAINTAINER) + "\n" + \
    tr("E-Mail") + ": " + QString::fromStdString(EMAIL) + "\n\n" + \
    tr("License") + ": " + QString::fromStdString(LICENSE);
}


/** Descrição do programa */
QString About::getDescription() {
    return "\n" + tr("Multimedia player developed in C++ using Qt5 as an interface. ") + \
    tr("To play multimedia files, the program uses the ") + "\n" + \
    tr("QtAV framework, which uses ffmpeg for handling multimedia files.");
}


/**********************************************************************************************************************/


/** Evento para capturar a posição da janela */
void About::mousePressEvent(QMouseEvent *event) {
    setPos = event->pos();
    QWidget::mousePressEvent(event);
}


/** Evento para mover a janela */
void About::mouseMoveEvent(QMouseEvent *event) {
    QPoint delta = QPoint(event->pos() - setPos);
    this->move(this->x() + delta.x(), this->y() + delta.y());
    QWidget::mouseMoveEvent(event);
}
