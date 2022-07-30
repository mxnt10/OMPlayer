#ifndef OMPLAYER_UTILS_H
#define OMPLAYER_UTILS_H

#include <QAction>

#include "Button.h"

#define VERSION "1.4.3"

#define GRE "\x1b[38;2;000;255;000m" //Debug
#define RED "\x1b[38;2;255;050;050m" //Debug
#define CYA "\x1b[38;2;000;255;255m" //Interface
#define SND "\x1b[38;2;040;125;200m" //Sender
#define ERR "\x1b[38;2;000;150;100m" //AVError
#define DGR "\x1b[38;2;000;150;020m" //Eventos
#define YEL "\x1b[38;2;255;255;000m" //Eventos
#define BLU "\x1b[38;2;000;120;255m" //Outros
#define VIO "\x1b[38;2;120;070;255m" //Posicionamentos
#define ORA "\x1b[38;2;255;100;000m" //Ações
#define EST "\x1b[38;2;180;125;000m" //Estilo
#define HID "\x1b[38;2;170;255;000m" //Ocultação
#define SHW "\x1b[38;2;085;255;050m" //Ocultação
#define UPD "\x1b[38;2;100;100;255m" //Atualizar
#define RDL "\x1b[38;2;255;100;100m" //Status

#define CENTER Qt::AlignCenter
#define TOP    Qt::AlignTop
#define BOTTON Qt::AlignBottom
#define RIGHT  Qt::AlignRight
#define LEFT   Qt::AlignLeft

#define CTRL  Qt::ControlModifier
#define ALT   Qt::AltModifier
#define ENTER Qt::Key_Return
#define ESC   Qt::Key_Escape

using namespace std;

class Utils {
public:
    static QString getLocal();
    static QString setIcon(bool logo = false);
    static QString setIconTheme(const QString &theme, const QString &icon);
    static QString setStyle(const QString &style);
    static QString mediaTitle(const QString &mediafile);
    static QString defaultIcon(const QString &icon);
    static QString setHash(const QString &url);
    static QString stringHash(const QString &url);
    static void arrowMouse();
    static void blankMouse();
    static void changeIcon(Button *btn, const QString &thm, const QString &ttp = nullptr);
    static void changeMenuIcon(QAction &btn, const QString &thm);
    static void rm_nl(string &s);
    static int calcX(int z, int x, int y);
    static int setDifere(int unit);
    static QStringList subdirIcons();
};

#endif //OMPLAYER_UTILS_H
