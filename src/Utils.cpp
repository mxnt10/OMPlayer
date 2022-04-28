#include <QApplication>
#include <QtCore>
#include <QDir>
#include <QFile>
#include <QString>
#include <filesystem>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#include "Utils.h"

using namespace std;
using std::filesystem::exists;


/**********************************************************************************************************************/


/**
 * O currentPath() pega o diretório corrente do binário do reprodutor, o que não é conveniente. Por isso, é usado
 * expressão regular para voltar um diretório que é o que desejamos. Esse recurso está disponível apenas à caráter
 * de testes de execução e depuração do reprodutor. */
QString Utils::getLocal() {
    return QDir::currentPath().remove(QRegularExpression("\\/(?:.(?!\\/))+$"));
}


/** Retorna o ícone do programa */
QString Utils::setIcon(bool logo) {
    QString icon = "/usr/share/pixmaps/OMPlayer.png";
    QString lIcon = getLocal() + "/appdata/OMPlayer.png";

    if (logo) {
        icon = "/usr/share/OMPlayer/logo/logo.png";
        lIcon = getLocal() + "/appdata/logo.png";
    }

    if (exists(icon.toStdString())) return icon;
    else if (exists(lIcon.toStdString())) return lIcon;
    return {};
}


/** Detecção de localização dos arquivos Qss */
QString Utils::setQssLocal(const string &file) {
    QString qFile = QString::fromStdString(file) + ".qss";
    QString qss = "/usr/share/OMPlayer/qss/" + qFile;
    QString lQss = getLocal() + "/qss/" + qFile;

    if (exists(qss.toStdString())) return qss;
    else if (exists(lQss.toStdString())) return lQss;
    return {};
}


/** Função que vai selecionar o tema dos ícones */
QString Utils::setIconTheme(const string &theme, const string &icon) {
    QString fileTheme = QString::fromStdString(theme) + "/" + QString::fromStdString(icon);
    QString iconFile = "/usr/share/OMPlayer/icons/" + fileTheme;
    QString lIconFile = getLocal() + "/icons/" + fileTheme;

    /** Compatibilidade com formatos svg e png */
    QString iconFileSVG = iconFile + ".svg";
    QString iconFilePNG = iconFile + ".png";
    QString lIconFileSVG = lIconFile + ".svg";
    QString lIconFilePNG = lIconFile + ".png";

    /** A função sempre vai dar prioridade aos arquivos svg */
    if (exists(iconFileSVG.toStdString())) return iconFileSVG;
    else if (exists(iconFilePNG.toStdString())) return iconFilePNG;
    else if (exists(lIconFileSVG.toStdString())) return lIconFileSVG;
    else if (exists(lIconFilePNG.toStdString())) return lIconFilePNG;
    else return {};
}


/** Função que retorna as configurações do estilo selecionado */
QString Utils::setStyle(const string &style) {
    QString qss = setQssLocal(style);
    QFile file(qss);
    file.open(QFile::ReadOnly);
    QString styleSheet{QLatin1String(file.readAll())};
    return styleSheet;
}


/** Definindo título do arquivo multimídia */
QString Utils::mediaTitle(const QString &mediafile){
    QString mediatitle = mediafile;

    /** Condição para definir o título na interface */
    if (!mediafile.contains(QLatin1String("://")) || mediafile.startsWith(QLatin1String("file://"))) {
        mediatitle = QFileInfo(mediafile).fileName();
    }
    return mediatitle;
}


/** Função criada para retornar os ícones equivalentes disponíveis no sistema */
QString Utils::defaultIcon(const string &icon) {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[33m Usando ícones do sistema para %s...\033[0m", icon.c_str());

    /** ícones dos controles de reprodução */
    if (icon == "play")
        return "media-playback-start";
    if (icon == "pause")
        return "media-playback-pause";
    if (icon == "stop")
        return "media-playback-stop";
    if (icon == "next")
        return "media-skip-forward";
    if (icon == "previous")
        return "media-skip-backward";
    if (icon == "replay")
        return "media-playlist-repeat";
    if (icon == "shuffle")
        return "media-playlist-shuffle";

    /** Ícones dos botões da playlist */
    if (icon == "add")
        return "list-add";
    if (icon == "remove")
        return "list-remove";
    if (icon == "clean")
        return "edit-delete";

    return{}; /** Se não estiver disponível, vai sem mesmo */
}


/** Função anti-bloqueio de tela usando recursos nativos do X11 */
void Utils::noBlockScreen() {
    qDebug("\033[32m(\033[31mDEBUG\033[32m):\033[36m Acionando anti-bloqueio ...\033[0m");
    Display *display;
    display = XOpenDisplay(nullptr);
    uint keycode = XKeysymToKeycode(display, XK_Control_L);
    XTestFakeKeyEvent(display, keycode, True, 0);
    XTestFakeKeyEvent(display, keycode, False, 0);
    XFlush(display);
}


/**********************************************************************************************************************/


/** Desocultar mouse */
void Utils::arrowMouse() {
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}


/** Ocultar mouse */
void Utils::blankMouse() {
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
}
