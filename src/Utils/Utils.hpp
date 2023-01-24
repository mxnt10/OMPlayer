#ifndef OMPLAYER_UTILS_H
#define OMPLAYER_UTILS_H

#include <QAction>
#include <QApplication>
#include <QLabel>
#include <QMenu>
#include <QPropertyAnimation>
#include <QPushButton>

#define VERSION "2023.01.24"
#define STR qUtf8Printable
#define FADE_VAL 200

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
    QString definedTheme;
} def;

class Utils {

public:
    enum AspectRatio {
        AspectVideo = 0, AspectAuto = 1, Aspect43 = 2, Aspect54 = 3, Aspect149 = 4, Aspect169 = 5, Aspect1610 = 6,
        Aspect235 = 7, Aspect11 = 8, Aspect32 = 9, Aspect1410 = 10, Aspect118 = 11, Aspect21 = 12
    };
    enum Init {DefineDir = 0, UpdateTheme = 2};
    enum Locals {OldDir = 0, Current = 1};

    static void initUtils(Utils::Init option = Utils::DefineDir);
    static QString setIcon();
    static QString setIconTheme(const QString &theme, const QString &icon);
    static QString setStyle(const QString &style);
    static QString mediaTitle(const QString &mediafile);
    static QString defaultIcon(const QString &icon);
    static QString setHash(const QString &url);
    static void changeIcon(QPushButton *btn, const QString &thm);
    static void changeIcon(QLabel *btn, const QString &thm);
    static void changeMenuIcon(QAction &btn, const QString &thm);
    static void changeMenuIcon(QMenu *btn, const QString &thm);
    static void rm_nl(string &s);
    static QStringList subdirIcons();
    static double aspectNum(Utils::AspectRatio option);
    static QString aspectStr(Utils::AspectRatio option);
    static QString setThemeName();
    static QStringList setLocals();
    static void fadeDiag(QPropertyAnimation *animation, float x, float y);
    static int calcX(int z, int x, int y) { return int(x / ((double)y / z)); }

private:
    static QString getLocal(Utils::Locals option = Utils::OldDir);
    static QString scanXDGData();
};

#endif //OMPLAYER_UTILS_H
