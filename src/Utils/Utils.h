#ifndef OMPLAYER_UTILS_H
#define OMPLAYER_UTILS_H

#include <QAction>
#include <QApplication>
#include <QMenu>
#include <Button>

#define VERSION "2.0.1"
#define STR qUtf8Printable

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

#define arrowMouse()  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor))
#define blankMouse()  QApplication::setOverrideCursor(QCursor(Qt::BlankCursor))
#define resizeMouse() QApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor))

#ifdef __linux__
    void notify_send(const char *title, const char *msg);
#endif

using namespace std;

struct { /** Criado com objetivo de otimização de performance */
    QString defaultDir;
    QString localDir;
    QString currentDir;
    QString definedXDG;
    QString definedIcon;
    QString definedLogo;
    QString definedNotify;
    QString definedTheme;
} def;

class Utils {

public:
    enum ASPECTRATIO {
        AspectAuto = 1, Aspect43 = 2, Aspect54 = 3, Aspect149 = 4, Aspect169 = 5, Aspect1610 = 6, Aspect235 = 7,
        Aspect11 = 8, Aspect32 = 9, Aspect1410 = 10, Aspect118 = 11, Aspect21 = 12, AspectVideo = 0
    };
    enum STATUS {Default = 0, Logo = 1, Notify = 2, Current = 3, Theme = 4};
    static void initUtils(Utils::STATUS option = Utils::Default);
    static QString setIcon(Utils::STATUS logo = Utils::Default);
    static QString setIconTheme(const QString &theme, const QString &icon);
    static QString setStyle(const QString &style);
    static QString mediaTitle(const QString &mediafile);
    static QString defaultIcon(const QString &icon);
    static QString setHash(const QString &url);
    static void changeIcon(Button *btn, const QString &thm);
    static void changeMenuIcon(QAction &btn, const QString &thm);
    static void changeMenuIcon(QMenu *btn, const QString &thm);
    static void rm_nl(string &s);
    static QStringList subdirIcons();
    static double aspectNum(Utils::ASPECTRATIO aspect);
    static QString aspectStr(Utils::ASPECTRATIO aspect);
    static QString setThemeName();
    static QStringList setLocals();
    static int calcX(int z, int x, int y) { return int(x / ((double)y / z)); }

private:
    static QString getLocal(Utils::STATUS option = Utils::Default);
    static QString defaultDir();
    static QString scanXDGData();
};

#endif //OMPLAYER_UTILS_H
