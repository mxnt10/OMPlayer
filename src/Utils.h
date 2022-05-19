#ifndef OMPLAYER_UTILS_H
#define OMPLAYER_UTILS_H

#include <string>

using namespace std;


/**
 * Classe para os utilitários.
 **********************************************************************************************************************/


class Utils {
public:
    static QString getLocal();
    static QString setIcon(bool logo = false);
    static QString setIconTheme(const string &theme, const string &icon);
    static QString setStyle(const string &style);
    static QString mediaTitle(const QString &mediafile);
    static QString defaultIcon(const string &icon);
    static void arrowMouse();
    static void blankMouse();
    static int calcX(int z, int x, int y);
    static int setDifere(int mUnit);

private:
    static QString setQssLocal(const string &file);
};

#endif //OMPLAYER_UTILS_H
