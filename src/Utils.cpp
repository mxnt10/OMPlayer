#include <QApplication>
#include <QDir>
#include <QFile>
#include <filesystem>

#include "JsonTools.h"
#include "Utils.h"

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


/** Função que vai selecionar o tema dos ícones */
QString Utils::setIconTheme(const QString &theme, const QString &icon) {
    QString fileTheme = theme + "/" + icon;
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
QString Utils::setStyle(const QString &style) {
    qDebug("%s(%sDEBUG%s):%s Selecionando estilo %s ...\033[0m", GRE, RED, GRE, EST, qUtf8Printable(style));

    QString qst = nullptr;
    QString qFile = style + ".qss";
    QString qss = "/usr/share/OMPlayer/qss/" + qFile;
    QString lQss = getLocal() + "/qss/" + qFile;

    if (exists(qss.toStdString())) qst = qss;
    else if (exists(lQss.toStdString())) qst = lQss;

    QFile file(qst);
    file.open(QFile::ReadOnly);
    QString styleSheet{QLatin1String(file.readAll())};
    return styleSheet;
}


/** Definindo título do arquivo multimídia */
QString Utils::mediaTitle(const QString &mediafile){
    QString mediatitle = mediafile;

    /** Condição para definir o título na interface */
    if (!mediafile.contains(QLatin1String("://")) || mediafile.startsWith(QLatin1String("file://")))
        mediatitle = QFileInfo(mediafile).fileName();
    return mediatitle;
}


/** Função criada para retornar os ícones equivalentes disponíveis no sistema */
QString Utils::defaultIcon(const QString &icon) {
    qDebug("%s(%sDEBUG%s):%s Usando ícone do sistema para %s ...\033[0m", GRE, RED, GRE, BLU, qUtf8Printable(icon));

    if (icon == "play")     return "media-playback-start";
    if (icon == "pause")    return "media-playback-pause";
    if (icon == "stop")     return "media-playback-stop";
    if (icon == "next")     return "media-skip-forward";
    if (icon == "previous") return "media-skip-backward";

    if (icon == "replay" || icon == "replay-menu")
        return "media-playlist-repeat";
    if (icon == "shuffle" || icon == "shuffle-menu")
        return "media-playlist-shuffle";

    if (icon == "add")    return "list-add";
    if (icon == "remove") return "list-remove";
    if (icon == "clean")  return "im-ban-kick-user";

    if (icon == "radio-select")   return "emblem-checked";
    if (icon == "radio-unselect") return "package-available";
    if (icon == "apply")          return "dialog-ok-apply";

    if (icon == "folder")     return "document-open-folder";
    if (icon == "fullscreen") return "view-fullscreen";
    if (icon == "settings")   return "configure";
    if (icon == "about")      return "help-about";

    return{}; /** Se não estiver disponível, vai sem mesmo */
}


/** Cálculo somente da largura do quadro uma vez que a altura vem pré-estabelecida */
int Utils::calcX(int z, int x, int y) {
    return x / (y / z);
}


/** Cálculo do diferença do intervalo de tempo */
int Utils::setDifere(int unit) {
    if (unit >= 250) return 500;
    return unit * 4;
}


/** Desocultar mouse */
void Utils::arrowMouse() {
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}


/** Ocultar mouse */
void Utils::blankMouse() {
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
}


/** Alteração dos botões */
void Utils::changeIcon(Button *btn, const QString &thm, const QString &ttp) {
    QString theme = JsonTools::readJson("theme");
    if (Utils::setIconTheme(theme, thm) == nullptr)
        btn->setIcon(QIcon::fromTheme(defaultIcon(thm)));
    else btn->setIcon(QIcon(Utils::setIconTheme(theme, thm)));
    if (!ttp.isNull()) btn->setToolTip(ttp);
}


/** Definição dos ícones do menu */
void Utils::changeMenuIcon(QAction &btn, const QString &thm) {
    QString theme = JsonTools::readJson("theme");
    if (Utils::setIconTheme(theme, thm) == nullptr)
        btn.setIcon(QIcon::fromTheme(defaultIcon(thm)));
    else btn.setIcon(QIcon(Utils::setIconTheme(theme, thm)));
}


/** Usado para tirar o "\n" dos debugs */
void Utils::rm_nl(string &s) {
    for (uint p = s.find('\n'); p != (uint) string::npos; p = s.find('\n')) s.erase(p, 1);
}
