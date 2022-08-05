#include <QLayout>
#include <QtAVWidgets>

#include "About.h"
#include "Label.h"
#include "Utils.h"


/**********************************************************************************************************************/


About::About(QWidget *parent) : QDialog(parent) {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet(Utils::setStyle("global")); // ToolTip
    setMaximumSize(0, 0);
    setModal(true);


    /** Nome do programa e descrição */
    auto *description = new Label(TOP, 0, nullptr, getDescription());
    auto *name = new Label(TOP, 0, nullptr, PRG_DESC);
    name->setStyleSheet("font-size: 24pt");


    /** Definição do icon logo */
    auto *iconlogo = new Label(CENTER);
    iconlogo->setPixmap(QPixmap(Utils::setIcon()));


    /** Versão do programa e demais informações*/
    auto *version = new Label(static_cast<const QFlag>(RIGHT | CENTER), 0, nullptr,
                              tr("Version") + " " + QString::fromStdString(VERSION));
    auto *maintainer = new Label(BOTTON, 0, nullptr, getTextMaintainer());
    version->setStyleSheet("font-size: 11pt");
    maintainer->setStyleSheet("font-size: 11pt");


    /** Botão para fechar a janela e para o sobre do QtAV */
    closebtn = new Button(Button::button, "apply", 32, tr("Close"));
    qtavbtn = new Button(Button::button, "info", 32, tr("About QtAV"));
    connect(closebtn, SIGNAL(pressed()), SLOT(onClose()));
    connect(qtavbtn, SIGNAL(pressed()), SLOT(setAboutQtAV()));


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
    qDebug("%s(%sDEBUG%s):%s Fechando o diálogo sobre ...\033[0m", GRE, RED, GRE, CYA);
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


/** Sobre o QtAV */
void About::setAboutQtAV() {
    QtAV::about();
}


/**********************************************************************************************************************/


/** Ajustes após a exibição das configurações */
void About::showEvent(QShowEvent *event) {
    Utils::changeIcon(closebtn, "apply");
    Utils::changeIcon(qtavbtn, "info");
    QDialog::showEvent(event);
}
