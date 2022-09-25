#include <QLayout>
#include <QtAVWidgets>

#include "About.h"
#include "Label.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Construtor que define a interface */
About::About(QWidget *parent) : QDialog(parent) {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMaximumSize(0, 0);
    setModal(true);


    /** Nome do programa e descrição */
    auto *description = new Label(TOP, 0, getDescription());
    auto *name = new Label(TOP, 0, PRG_DESC);
    name->setStyleSheet("font-size: 24pt");


    /** Definição do icon logo */
    auto *iconlogo = new Label(CENTER, 0, 0);
    iconlogo->setPixmap(QPixmap(Utils::setIcon()));


    /** Versão do programa e demais informações*/
    auto *version = new Label(static_cast<const QFlag>(RIGHT | CENTER), 0,
                              tr("Version") + " " + QString::fromStdString(VERSION));
    auto *maintainer = new Label(BOTTON, 0, getTextMaintainer());
    version->setStyleSheet("font-size: 11pt");
    maintainer->setStyleSheet("font-size: 11pt");


    /** Botão para fechar a janela e para o sobre do QtAV */
    closebtn = new Button(Button::button, "apply", 32, tr("Close"));
    qtavbtn = new Button(Button::button, "info", 32, tr("About QtAV"));
    connect(closebtn, &Button::clicked, this, &About::onClose);
    connect(qtavbtn, &Button::clicked, this, &QtAV::about);


    /** Layout dos botões */
    auto *buttons = new QHBoxLayout();
    buttons->addWidget(qtavbtn);
    buttons->setSpacing(5);
    buttons->addWidget(closebtn);


    /** Organização das informações do Sobre */
    auto *labout = new QGridLayout();
    labout->setMargin(20);
    labout->addWidget(name,        0, 0);
    labout->addWidget(version,     0, 1, RIGHT | CENTER);
    labout->addWidget(description, 1, 0, 1, 2);
    labout->addWidget(iconlogo,    2, 0, 1, 2);
    labout->addWidget(maintainer,  3, 0);
    labout->addLayout(buttons,     3, 1, RIGHT | BOTTON);


    /** Widget para o plano de fundo */
    auto *found = new QWidget();
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
    qDebug("%s(%sAbout%s)%s::%sFechando o diálogo sobre ...\033[0m", GRE, RED, GRE, RED, CYA);
    emit emitclose();
    this->close();
}


/** Informações adicionais */
QString About::getTextMaintainer() {
    return tr("Maintainer") + ": " + QString::fromStdString(MAINTAINER) + "\n" + \
    tr("E-Mail") + ": " + QString::fromStdString(EMAIL) + "\n\n" + \
    tr("License") + ": " + QString::fromStdString(LICENSE);
}


/** Descrição do programa */
QString About::getDescription() {
    return "\n" + tr("Multimedia player developed in C++ using Qt5 as an interface.") + " " + \
    tr("To play multimedia files, the program uses the") + " \n" + \
    tr("QtAV framework, which uses ffmpeg for handling multimedia files.");
}


/** Alterando o tema dos ícones */
void About::changeIcons() {
    Utils::changeIcon(closebtn, "apply");
    Utils::changeIcon(qtavbtn, "info");
}
