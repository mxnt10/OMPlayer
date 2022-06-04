#ifndef OMPLAYER_UTILS_H
#define OMPLAYER_UTILS_H

#include <QAction>

#include "Button.h"

using namespace std;


/**
 * Classe para os utilitários.
 **********************************************************************************************************************/


class Utils {
public:
    static QString getLocal();
    static QString setIcon(bool logo = false);
    static QString setIconTheme(const QString &theme, const QString &icon);
    static QString setStyle(const QString &style);
    static QString mediaTitle(const QString &mediafile);
    static QString defaultIcon(const QString &icon);
    static void arrowMouse();
    static void blankMouse();
    static int calcX(int z, int x, int y);
    static int setDifere(int unit);
    static void changeIcon(Button *btn, const QString &thm);
    static void changeMenuIcon(QAction &btn, const QString &thm);
};

#endif //OMPLAYER_UTILS_H
