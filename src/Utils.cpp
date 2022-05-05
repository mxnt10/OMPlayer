#include <QApplication>
#include <QtCore>
#include <QDir>
#include <QFile>
#include <QString>
#include <filesystem>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#include "Defines.h"
#include "Utils.h"

using namespace std;
using std::filesystem::exists;


/**********************************************************************************************************************/


/**
 * O currentPath() pega o diretório corrente do binário do reprodutor, o que não é conveniente. Por isso, é usado
 * expressão regular para voltar um diretório que é o que desejamos. Esse recurso está disponível apenas à caráter
 * de testes de execução e depuração do reprodutor. */
QString Utils::getLocal() {
    return QDir::currentPath().remove(QRegExp("\\/(?:.(?!\\/))+$"));
}


/** Retorna o ícone do programa */
QString Utils::setIcon(bool logo) {
    QString icon = "/usr/share/pixmaps/OMPlayer.png";
    QString lIcon = getLocal() + "/appdata/OMPlayer.png";

    if (logo) {
        qDebug("%s(%sDEBUG%s):%s Setando uma logo ...\033[0m", GRE, RED, GRE, BLU);
        icon = "/usr/share/OMPlayer/logo/logo.png";
        lIcon = getLocal() + "/appdata/logo.png";
    } else
        qDebug("%s(%sDEBUG%s):%s Setando o ícone do programa ...\033[0m", GRE, RED, GRE, BLU);

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
    qDebug("%s(%sDEBUG%s):%s Selecionando estilo %s ...\033[0m", GRE, RED, GRE, EST, style.c_str());
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
    qDebug("%s(%sDEBUG%s):%s Usando ícone do sistema para %s ...\033[0m", GRE, RED, GRE, BLU, icon.c_str());

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
    qDebug("%s(%sDEBUG%s):%s Acionando anti-bloqueio ...\033[0m", GRE, RED, GRE, BLU);
    Display *display;
    display = XOpenDisplay(nullptr);
    uint keycode = XKeysymToKeycode(display, XK_Control_L);
    while (true) {
        XTestFakeKeyEvent(display, keycode, True, 0);
        XTestFakeKeyEvent(display, keycode, False, 0);
        XFlush(display);
        if (QThread::currentThread()->isInterruptionRequested()) {
            qDebug("%s(%sDEBUG%s):%s Parando anti-bloqueio ...\033[0m", GRE, RED, GRE, BLU);
            return;
        }
        sleep(15);
    }
}


/** Cálculo somente da largura do quadro uma vez que a altura vem pré-estabelecida */
int Utils::calcX(int z, int x, int y) {
    return x / (y / z);
}


/** Cálculo do diferença do intervalo de tempo */
int Utils::setDifere(int mUnit) {
    if (mUnit == 500) return mUnit;
    if (mUnit == 250) return mUnit * 2;
    if (mUnit <= 47) return mUnit * 4;
    return mUnit * 3;
}


/** Desocultar mouse */
void Utils::arrowMouse() {
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}


/** Ocultar mouse */
void Utils::blankMouse() {
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
}
