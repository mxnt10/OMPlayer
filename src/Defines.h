#ifndef OMPLAYER_DEFINES_H
#define OMPLAYER_DEFINES_H


/** Depuração */
#define DEBUG false


/** Defines para o sobre */
#define PRG_NAME "Open Multimedia Player"
#define PRG_DESC "OMPlayer - Open Multimedia Player"
#define VERSION "1.2"
#define MAINTAINER "Mauricio Ferrari"
#define EMAIL "m10ferrari1200@gmail.com"
#define LICENSE "GNU General Public License Version 3 (GLPv3)"


/** Defines para emissões de sinal repeteco */
#define EMITENTER SIGNAL(emitEnter())
#define EMITLEAVE SIGNAL(emitLeave())
#define TRIGGERED SIGNAL(triggered())
#define ACTIVATED SIGNAL(activated())
#define PRESSED SIGNAL(pressed())
#define CLICKED SIGNAL(clicked())


/** Defines de Align */
#define CENTER Qt::AlignCenter
#define TOP    Qt::AlignTop
#define BOTTON Qt::AlignBottom
#define RIGHT  Qt::AlignRight


/** Defines de botões */
#define CTRL  Qt::ControlModifier
#define ALT   Qt::AltModifier
#define ENTER Qt::Key_Return
#define ESC   Qt::Key_Escape


/** Outros */
#define OPACY 0.8


/**
 * Classe para auxílio dos defines.
 **********************************************************************************************************************/


class About {
public:
    static QString getTextMaintainer();

    static QString getDescription();
};

#endif //OMPLAYER_DEFINES_H
