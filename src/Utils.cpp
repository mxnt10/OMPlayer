#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFile>

#include <cstdlib>

#include "JsonTools.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Para otimização de performance. Seta os locais padrão para a busca por recursos para o programa. */
void Utils::initUtils(Status option) {
    if (option == Default) {
        qDebug("%s(%sUtils%s)%s::%sDefinição de locais padrão ...\033[0m", GRE, RED, GRE, RED, ORA);
        def.defaultDir = defaultDir();
        def.localDir = getLocal();
        def.currentDir = getLocal(Current);
    }
    def.definedTheme = JsonTools::stringJson("theme");
}


/**
 * O currentPath() pega o diretório corrente do binário do reprodutor, o que não é conveniente. Por isso, é usado
 * expressão regular para voltar um diretório que é o que desejamos. Esse recurso está disponível apenas à caráter
 * de testes de execução e depuração do reprodutor. */
QString Utils::getLocal(Status option) {
    if (option == Current) return QDir::currentPath();
    return QDir::currentPath().remove(QRegExp("\\/(?:.(?!\\/))+$"));
}


/** Diretório padrão e retorno de definição de locais padrão */
QStringList Utils::setLocals() { return {def.defaultDir, def.localDir, def.currentDir}; }


/** Diretório padrão do programa */
QString Utils::defaultDir() { return scanXDGData() + "/usr/share/OMPlayer"; }


/** Retorna o ícone do programa */
QString Utils::setIcon(Status logo) {
    QStringList locals;
    QString iconfile, icon;

    if (logo == Logo) {
        if (!def.definedLogo.isEmpty() && QFileInfo::exists(def.definedLogo)) return def.definedLogo;
        qDebug("%s(%sUtils%s)%s::%sSetando uma logo ...\033[0m", GRE, RED, GRE, RED, BLU);
        locals.append({def.defaultDir + "/logo", def.localDir + "/appdata", def.currentDir + "/appdata"});
        icon = "/logo.png";
    } else {
        if (!def.definedIcon.isEmpty() && QFileInfo::exists(def.definedIcon)) return def.definedIcon;
        qDebug("%s(%sUtils%s)%s::%sSetando o ícone do programa ...\033[0m", GRE, RED, GRE, RED, BLU);
        locals.append({def.definedXDG + "/usr/share/pixmaps", def.localDir + "/appdata", def.currentDir + "/appdata"});
        icon = "/OMPlayer.png";
    }

    for (int i = 0; i < 3; i++) {
        iconfile = locals[i] + icon;
        if (QFileInfo::exists(iconfile)) {
            if (logo == Logo) def.definedLogo = iconfile; else def.definedIcon = iconfile;
            return iconfile;
        }
    }
    return {};
}


/** Função que vai selecionar o tema dos ícones */
QString Utils::setIconTheme(const QString &theme, const QString &icon) {
    QStringList locals{def.defaultDir, def.localDir, def.currentDir};
    QStringList files;
    QString iconDir;
    QDir dir;

    for (int i = 0; i < 3; i++) {
        iconDir = locals[i] + "/icons/" + theme + "/";
        dir.setPath(iconDir);
        files = dir.entryList();

        if (!files.filter(QRegExp("^" + icon + "\\.(pn|sv)g")).isEmpty())
            return iconDir + files.filter(QRegExp("^" + icon + "\\.(pn|sv)g"))[0];
    }

    return {};
}


/** Função que retorna as configurações do estilo selecionado */
QString Utils::setStyle(const QString &style) {
    qDebug("%s(%sUtils%s)%s::%sUsando estilo: %s%s ...\033[0m", GRE, RED, GRE, RED, EST, SHW, qUtf8Printable(style));
    QStringList locals{def.defaultDir, def.localDir, def.currentDir};
    QString qss;

    for (int i = 0; i < 3; i++) {
        if (QFileInfo::exists(locals[i] + "/qss/" + style + ".qss")) {
            qss = locals[i] + "/qss/" + style + ".qss";
            break;
        }
    }

    if (qss.isEmpty()) {
        if (style == "widget") {
            if (QRegExp("dark|-dark").indexIn(QIcon::themeName()) != -1) {
                return {"background-color: rgba(42, 46, 50, 255);"
                        "border: 1px ridge #43474c;"
                        "border-radius: 4px;"};
            } else {
                return {"background-color: rgba(239, 240, 241, 255);"
                        "border: 1px ridge #ebebeb;"
                        "border-radius: 4px;"};
            }
        } else return {};
    }

    QFile file(qss);
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
    if (icon.isEmpty() ) {
        qDebug("%s(%sUtils%s)%s::%sPulando ícones não definidos ...\033[0m", GRE, RED, GRE, RED, BLU);
        return {};
    }

    qDebug("%s(%sUtils%s)%s::%sUsando ícone do sistema: %s ...\033[0m", GRE, RED, GRE, RED, BLU, qUtf8Printable(icon));
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


/** Alteração dos botões */
void Utils::changeIcon(Button *btn, const QString &thm, const QString &ttp) {
    if (Utils::setIconTheme(def.definedTheme, thm) == nullptr)
        btn->setIcon(QIcon::fromTheme(defaultIcon(thm)));
    else btn->setIcon(QIcon(Utils::setIconTheme(def.definedTheme, thm)));
    if (!ttp.isNull()) btn->setToolTip(ttp);
}


/** Definição dos ícones do menu */
void Utils::changeMenuIcon(QAction &btn, const QString &thm) {
    if (Utils::setIconTheme(def.definedTheme, thm) == nullptr)
        btn.setIcon(QIcon::fromTheme(defaultIcon(thm)));
    else btn.setIcon(QIcon(Utils::setIconTheme(def.definedTheme, thm)));
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
    qDebug("%s(%sUtils%s)%s::%sBuscando temas ...\033[0m", GRE, RED, GRE, RED, HID);
    QStringList locals{def.defaultDir, def.localDir, def.currentDir};
    QDir dir;
    QStringList dirs;

    for (int i = 0; i < 3; i++) {
        dir.setPath(locals[i].append("/icons/"));
        if (dir.exists()) dirs = dir.entryList(QDir::Dirs);
    }

    for (int i = 0; i < 2; i++) dirs.removeAt(0); /** Removendo os itens "." e ".." indesejáveis */
    return dirs;
}


/** Função para o escaneamento da variável de ambiente XDG_DATA_DIRS */
QString Utils::scanXDGData() {
    const char *tmp = getenv("XDG_DATA_DIRS");
    QString env(tmp ? tmp : "");
    if (env.isEmpty()) return {};

    QStringList splitt = env.split(":");
    for (int i = 0; i < splitt.size(); i ++) {
        QDir dir(splitt[i] + "/usr/share/OMPlayer");
        if (dir.exists()) {
            qDebug("%s(%sUtils%s)%s::%sUsando XDG_DATA_DIRS ...\033[0m", GRE, RED, GRE, RED, HID);
            def.definedXDG = splitt[i];
            return splitt[i];
        }
    }
    return {};
}


/** Retorno das dimensões de tela */
double Utils::aspectNum(Aspect aspect) {
    switch (aspect) {
        case Utils::AspectVideo:  return 0;
        case Utils::Aspect21:     return (double) 2 / 1;
        case Utils::Aspect43:     return (double) 4 / 3;
        case Utils::Aspect169:    return (double) 16 / 9;
        case Utils::Aspect149:    return (double) 14 / 9;
        case Utils::Aspect1610:   return (double) 16 / 10;
        case Utils::Aspect54:     return (double) 5 / 4;
        case Utils::Aspect235:    return 2.35;
        case Utils::Aspect11:     return 1;
        case Utils::Aspect32:     return (double) 3 / 2;
        case Utils::Aspect1410:   return (double) 14 / 10;
        case Utils::Aspect118:    return (double) 11 / 8;
        case Utils::AspectAuto:   return (-1);
        default: return 0;
    }
}


/** Retorna o texto das dimensões de tela */
QString Utils::aspectStr(Aspect aspect) {
    switch (aspect) {
        case Utils::AspectVideo:  return QApplication::tr("Video");
        case Utils::Aspect21:     return "2:1";
        case Utils::Aspect43:     return "4:3";
        case Utils::Aspect169:    return "16:9";
        case Utils::Aspect149:    return "14:9";
        case Utils::Aspect1610:   return "16:10";
        case Utils::Aspect54:     return "5:4";
        case Utils::Aspect235:    return "2.35:1";
        case Utils::Aspect11:     return "1:1";
        case Utils::Aspect32:     return "3:2";
        case Utils::Aspect1410:   return "14:10";
        case Utils::Aspect118:    return "11:8";
        case Utils::AspectAuto:   return QObject::tr("Window");
        default: return QApplication::tr("Unknown");
    }
}
