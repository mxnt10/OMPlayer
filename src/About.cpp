#include <QLayout>
#include <QTimer>
#include <QtAVWidgets>
#include <Label>
#include <Utils>

#include "About.hpp"


/**********************************************************************************************************************/


/** Construtor que define a interface */
About::About(QWidget *parent) : Dialog(parent) {
    setMaximumSize(0, 0);
    connect(this, &Dialog::emitclose, [this](){ Q_EMIT onclose(); });


    /** Nome do programa e descrição e ícone */
    auto *iconlogo = new Label(CENTER, Utils::setIcon());
    auto *description = new Label(TOP, null, getDescription());
    auto *name = new Label(TOP, null, PRG_DESC);
    name->setStyleSheet("font-size: 22pt");


    /** Versão do programa e demais informações*/
    auto *version = new Label(static_cast<const QFlag>(RIGHT | CENTER), null, VERSION);
    auto *maintainer = new Label(BOTTON, null, getTextMaintainer());
    version->setStyleSheet("font-size: 12pt");
    maintainer->setStyleSheet("font-size: 11pt");


    /** Botão para fechar a janela e para o sobre do QtAV */
    closebtn = new Button(Button::NormalBtn, 32, "apply");
    qtavbtn = new Button(Button::NormalBtn, 32, "info");
    connect(closebtn, &Button::clicked, this, &Dialog::onClose);
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


/** Informações adicionais */
QString About::getTextMaintainer() {
    return tr("Maintainer") + ": " + MAINTAINER + "\n" + \
    tr("E-Mail") + ": " + EMAIL + "\n\n" + tr("License") + ": " + LICENSE;
}


/** Descrição do programa */
QString About::getDescription() {
    return tr("Multimedia player developed in C++ using Qt5 as an interface.") + " " + \
    tr("To play multimedia files, the program uses the") + " \n" + \
    tr("QtAV framework, which uses ffmpeg for handling multimedia files.") + "\n";
}


/** Alterando o tema dos ícones */
void About::changeIcons() {
    Utils::changeIcon(closebtn, "apply");
    Utils::changeIcon(qtavbtn, "info");
}
