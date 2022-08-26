#include <QApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFile>

#include <cstdlib>

#include "JsonTools.h"
#include "Utils.h"


/**********************************************************************************************************************/


/**
 * O currentPath() pega o diretório corrente do binário do reprodutor, o que não é conveniente. Por isso, é usado
 * expressão regular para voltar um diretório que é o que desejamos. Esse recurso está disponível apenas à caráter
 * de testes de execução e depuração do reprodutor. */
QString Utils::getLocal(ST option) {
    if (option == Current) return QDir::currentPath();
    return QDir::currentPath().remove(QRegExp("\\/(?:.(?!\\/))+$"));
}


/** Diretório padrão do programa */
QString Utils::defaultDir() { return scanXDGData() + "/usr/share/OMPlayer"; }


/** Retorna o ícone do programa */
QString Utils::setIcon(ST logo) {
    QString icon, lIcon;
    if (logo == Logo) {
        qDebug("%s(%sDEBUG%s):%s Setando uma logo ...\033[0m", GRE, RED, GRE, BLU);
        icon = defaultDir() + "/logo/logo.png";
        lIcon = getLocal() + "/appdata/logo.png";
    } else {
        qDebug("%s(%sDEBUG%s):%s Setando o ícone do programa ...\033[0m", GRE, RED, GRE, BLU);
        icon = scanXDGData() + "/usr/share/pixmaps/OMPlayer.png";
        lIcon = getLocal() + "/appdata/OMPlayer.png";
    }

    if (QFileInfo::exists(icon)) return icon;
    else if (QFileInfo::exists(lIcon)) return lIcon;
    else {
        if (logo == Logo) lIcon = getLocal(Current) + "/appdata/logo.png";
        else lIcon = getLocal(Current) + "/appdata/OMPlayer.png";
        if (QFileInfo::exists(lIcon)) return lIcon;
    }
    return {};
}


/** Função que vai selecionar o tema dos ícones */
QString Utils::setIconTheme(const QString &theme, const QString &icon) {
    QString fileTheme{theme + "/" + icon};
    QString iconFile{defaultDir() + "/icons/" + fileTheme};
    QString lIconFile{getLocal() + "/icons/" + fileTheme};

    /** Compatibilidade com formatos svg e png */
    QString iconFileSVG{iconFile + ".svg"};
    QString iconFilePNG{iconFile + ".png"};
    QString lIconFileSVG{lIconFile + ".svg"};
    QString lIconFilePNG{lIconFile + ".png"};

    /** Prioridade aos arquivos svg */
    if (QFileInfo::exists(iconFileSVG)) return iconFileSVG;
    else if (QFileInfo::exists(iconFilePNG)) return iconFilePNG;
    else if (QFileInfo::exists(lIconFileSVG)) return lIconFileSVG;
    else if (QFileInfo::exists(lIconFilePNG)) return lIconFilePNG;
    else {
        lIconFile = getLocal(Current) + "/icons/" + fileTheme;
        lIconFileSVG = lIconFile + ".svg";
        lIconFilePNG = lIconFile + ".png";
        if (QFileInfo::exists(lIconFileSVG)) return lIconFileSVG;
        if (QFileInfo::exists(lIconFilePNG)) return lIconFilePNG;
    }
    return {};
}


/** Função que retorna as configurações do estilo selecionado */
QString Utils::setStyle(const QString &style) {
    qDebug("%s(%sDEBUG%s):%s Selecionando estilo %s ...\033[0m", GRE, RED, GRE, EST, qUtf8Printable(style));
    QString qss{defaultDir() + "/qss/" + style + ".qss"};
    QString lQss{getLocal() + "/qss/" + style + ".qss"};

    QString qst;
    if (QFileInfo::exists(qss)) qst = qss;
    else if (QFileInfo::exists(lQss)) qst = lQss;
    else {
        lQss = getLocal(Current) + "/qss/" + style + ".qss";
        if (QFileInfo::exists(lQss)) qst = lQss;
    }

    if (qst.isEmpty() && style == "widget") {
        if (QRegExp("dark|-dark").indexIn(QIcon::themeName()) != -1) {
            return {"background-color: rgba(42, 46, 50, 255);"
                    "border: 1px ridge #43474c;"
                    "border-radius: 4px;"};
        } else {
            return {"background-color: rgba(239, 240, 241, 255);"
                    "border: 1px ridge #ebebeb;"
                    "border-radius: 4px;"};
        }
    }

    if (qst.isEmpty()) return {};

    QFile file(qst);
    if (!file.open(QFile::ReadOnly)) return {};
    QString styleSheet{QLatin1String(file.readAll())};
    file.close();
    return styleSheet;
}


/** Definindo título do arquivo multimídia */
QString Utils::mediaTitle(const QString &mediafile){
    if (!mediafile.contains(QLatin1String("://")) || mediafile.startsWith(QLatin1String("file://")))
        return QFileInfo(mediafile).fileName();
    return mediafile;
}


/** Função criada para retornar os ícones equivalentes disponíveis no sistema */
QString Utils::defaultIcon(const QString &icon) {
    qDebug("%s(%sDEBUG%s):%s Usando ícone do sistema para %s ...\033[0m", GRE, RED, GRE, BLU, qUtf8Printable(icon));

    if (icon == "play")           return "media-playback-start";
    if (icon == "pause")          return "media-playback-pause";
    if (icon == "stop")           return "media-playback-stop";
    if (icon == "next")           return "media-skip-forward";
    if (icon == "previous")       return "media-skip-backward";
    if (icon == "repeat-on")      return "media-repeat-single";
    if (icon == "repeat")         return "media-repeat-none";
    if (icon == "volume_high")    return "audio-volume-high";
    if (icon == "volume_medium")  return "audio-volume-medium";
    if (icon == "volume_low")     return "audio-volume-low";
    if (icon == "add")            return "list-add";
    if (icon == "remove")         return "list-remove";
    if (icon == "clean")          return "im-ban-kick-user";
    if (icon == "radio-select")   return "emblem-checked";
    if (icon == "radio-unselect") return "package-available";
    if (icon == "apply")          return "dialog-ok-apply";
    if (icon == "folder")         return "document-open-folder";
    if (icon == "fullscreen")     return "view-fullscreen";
    if (icon == "settings")       return "configure";

    if (QRegExp("mute|nosound").indexIn(icon) != -1)          return "audio-volume-muted";
    if (QRegExp("replay($|-(on|menu))").indexIn(icon) != -1)  return "media-playlist-repeat";
    if (QRegExp("shuffle($|-(on|menu))").indexIn(icon) != -1) return "media-playlist-shuffle";
    if (QRegExp("about|info").indexIn(icon) != -1)            return "help-about";

    return {}; /** Se não estiver disponível, vai sem mesmo */
}


/** Cálculo do diferença do intervalo de tempo */
int Utils::setDifere(int unit) { if (unit >= 100) return 500; return unit * 6; }


/** Cálculo somente da largura do quadro uma vez que a altura vem pré-estabelecida */
int Utils::calcX(int z, int x, int y) { return int(x / ((double)y / z)); }


/** Usado para tirar o "\n" dos debugs */
void Utils::rm_nl(string &s) { for (uint p = s.find('\n'); p != (uint) string::npos; p = s.find('\n')) s.erase(p, 1); }


/** Funções para o cursor do mouse */
void Utils::arrowMouse()  { QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));   }
void Utils::blankMouse()  { QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));   }
void Utils::resizeMouse() { QApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor)); }


/** Alteração dos botões */
void Utils::changeIcon(Button *btn, const QString &thm, const QString &ttp) {
    QString theme = JsonTools::stringJson("theme");
    if (Utils::setIconTheme(theme, thm) == nullptr)
        btn->setIcon(QIcon::fromTheme(defaultIcon(thm)));
    else btn->setIcon(QIcon(Utils::setIconTheme(theme, thm)));
    if (!ttp.isNull()) btn->setToolTip(ttp);
}


/** Definição dos ícones do menu */
void Utils::changeMenuIcon(QAction &btn, const QString &thm) {
    QString theme = JsonTools::stringJson("theme");
    if (Utils::setIconTheme(theme, thm) == nullptr)
        btn.setIcon(QIcon::fromTheme(defaultIcon(thm)));
    else btn.setIcon(QIcon(Utils::setIconTheme(theme, thm)));
}


/** Capturando o hash MD5 de um arquivo */
QString Utils::setHash(const QString &url) {
    QCryptographicHash hash(QCryptographicHash::Md5);
    QFile f(url);
    QFileInfo fi(url);

    int fsize = int(fi.size());
    int bsize = 8192; //8KiB

    if (f.open(QIODevice::ReadOnly)) {
        char buf[bsize];
        int bread;
        int rsize = qMin(fsize, bsize);

        /** Capturando o hash */
        while (rsize > 0 && (bread = int(f.read(buf, rsize))) > 0) {
            fsize -= bread;
            hash.addData(buf, bread);
            rsize = qMin(fsize, bsize);
        }

        f.close();
        return hash.result().toHex();
    } else return {};
}


/** Capturando o hash MD5 de uma string */
QString Utils::stringHash(const QString &url) {
    return QCryptographicHash::hash(url.toLocal8Bit(), QCryptographicHash::Md5).toHex();
}


/** Função que retorna os subdiretórios presente em icons */
QStringList Utils::subdirIcons() {
    QDir dir(defaultDir().append("/icons/"));
    QDir ldir(getLocal().append("/icons/"));
    QStringList dirs;

    if (dir.exists()) dirs = dir.entryList(QDir::Dirs);
    else if (ldir.exists()) dirs = ldir.entryList(QDir::Dirs);
    else {
        ldir.setPath(getLocal(Current).append("/icons/"));
        if (ldir.exists()) dirs = ldir.entryList(QDir::Dirs);
        else return {};
    }

    for (int i = 0; i < 2; i++) dirs.removeAt(0); /** Removendo os itens "." e ".." indesejáveis */
    return dirs;
}


/** Função para o escaneamento da variável de ambiente XDG_DATA_DIRS */
QString Utils::scanXDGData() {
    const char *tmp = getenv("XDG_DATA_DIRS");
    QString env(tmp ? tmp : "");
    QStringList splitt = env.split(":");
    for (int i = 0; i < splitt.size(); i ++) {
        QDir dir(splitt[i] + "/usr/share/OMPlayer");
        if (dir.exists()) return splitt[i];
    }
    return {};
}
