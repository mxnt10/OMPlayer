#ifndef OMPLAYER_DEFINES_H
#define OMPLAYER_DEFINES_H
#include <QString>


/** Depuração */
#define DEBUG false


/** Definição de cores para o Debug em RGB: \x1b[38;2;000;000;000m */
#define GRE "\x1b[38;2;000;255;000m" //Debug
#define RED "\x1b[38;2;255;050;050m" //Debug

#define CYA "\x1b[38;2;000;255;255m" //Interface

#define DGR "\x1b[38;2;000;150;020m" //Eventos
#define YEL "\x1b[38;2;255;255;000m" //Eventos

#define BLU "\x1b[38;2;000;120;255m" //Outros

#define VIO "\x1b[38;2;120;070;255m" //Posicionamentos
#define ORA "\x1b[38;2;255;100;000m" //Ações

#define EST "\x1b[38;2;180;125;000m" //Estilo

#define HID "\x1b[38;2;170;255;000m" //Ocultação
#define SHW "\x1b[38;2;085;255;050m" //Ocultação

#define ADD "\x1b[38;2;255;100;100m" //Adicionar
#define UPD "\x1b[38;2;100;100;255m" //Atualizar


#define PRG_NAME "Open Multimedia Player"
#define PRG_DESC "OMPlayer - Open Multimedia Player"
#define VERSION "1.3.1"
#define MAINTAINER "Mauricio Ferrari"
#define EMAIL "m10ferrari1200@gmail.com"
#define LICENSE "GNU General Public License Version 3 (GLPv3)"

#define CENTER Qt::AlignCenter
#define TOP    Qt::AlignTop
#define BOTTON Qt::AlignBottom
#define RIGHT  Qt::AlignRight

#define CTRL  Qt::ControlModifier
#define ALT   Qt::AltModifier
#define ENTER Qt::Key_Return
#define ESC   Qt::Key_Escape

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
