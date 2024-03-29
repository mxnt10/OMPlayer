#include <QCryptographicHash>
#include <QDir>
#include <QFile>

#include <cstdlib>

#include "JsonTools.hpp"
#include "Utils.hpp"


/**********************************************************************************************************************/


#ifdef __linux__
    #undef signals
    #include <libnotify/notify.h>

    /** Exibição de notificações do sistema usando libnotify */
    void notify_send(const char *title, const char *msg) {
        NotifyNotification *n;
        notify_init("Open Multimedia Player");

        n = notify_notification_new(title, msg, nullptr);
        notify_notification_set_timeout(n, 5000);

        if (!notify_notification_show(n, nullptr)) return;

        g_object_unref(G_OBJECT(n));
    }

    #define signals public
#endif


/** Para otimização de performance. Seta os locais padrão para a busca por recursos para o programa. */
void Utils::initUtils(Utils::Init option) {
    if (option == Utils::DefineDir) {
        qDebug("%s(%sUtils%s)%s::%sDefinição de locais padrão ...\033[0m", GRE, RED, GRE, RED, ORA);
        def.defaultDir = scanXDGData() + "/usr/share/OMPlayer";
        def.localDir = getLocal();
        def.currentDir = getLocal(Utils::Current);
    }
    def.definedTheme = JsonTools::stringJson("theme");
}


/**
 * O currentPath() pega o diretório corrente do binário do reprodutor, o que não é tão conveniente. Por isso, é usado
 * expressão regular para voltar um diretório que é o que desejamos. Esse recurso está disponível à caráter
 * de testes de execução e depuração do reprodutor e em caso de portabilizar o reprodutor. */
QString Utils::getLocal(Utils::Locals option) {
    if (option == Utils::Current) return QDir::currentPath();
    return QDir::currentPath().remove(QRegExp("\\/(?:.(?!\\/))+$"));
}


/** Diretórios padrão do programa */
QStringList Utils::setLocals() { return {def.defaultDir, def.localDir, def.currentDir}; }


/** Retorna o ícone do programa */
QString Utils::setIcon() {
    qDebug("%s(%sUtils%s)%s::%sSetando o ícone do programa ...\033[0m", GRE, RED, GRE, RED, BLU);
    if (!def.definedIcon.isEmpty() && QFileInfo::exists(def.definedIcon)) return def.definedIcon;

    QStringList locals{def.definedXDG + "/usr/share/pixmaps", def.localDir + "/appdata", def.currentDir + "/appdata"};

    for (int i = 0; i < 3; i++) {
        if (QFileInfo::exists(locals[i] + "/OMPlayer.png")) {
            def.definedIcon = locals[i] + "/OMPlayer.png";
            return def.definedIcon;
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

    qDebug("%s(%sUtils%s)%s::%sUsando ícone do sistema: %s ...\033[0m", GRE, RED, GRE, RED, BLU, STR(icon));
    if (icon == "play")            return "media-playback-start";
    if (icon == "pause")           return "media-playback-pause";
    if (icon == "stop")            return "media-playback-stop";
    if (icon == "replay-one-menu") return "media-repeat-single";
    if (icon == "volume_high")     return "audio-volume-high";
    if (icon == "volume_medium")   return "audio-volume-medium";
    if (icon == "volume_low")      return "audio-volume-low";
    if (icon == "add")             return "list-add";
    if (icon == "remove")          return "list-remove";
    if (icon == "clean")           return "im-ban-kick-user";
    if (icon == "apply")           return "dialog-ok-apply";
    if (icon == "folder")          return "document-open-folder";
    if (icon == "fullscreen")      return "view-fullscreen";
    if (icon == "settings")        return "configure";

    if (QRegExp("next|forward").indexIn(icon) != -1)          return "media-skip-forward";
    if (QRegExp("previous|backward").indexIn(icon) != -1)     return "media-skip-backward";
    if (QRegExp("mute|nosound").indexIn(icon) != -1)          return "audio-volume-muted";
    if (QRegExp("replay($|-(on|menu))").indexIn(icon) != -1)  return "media-playlist-repeat";
    if (QRegExp("shuffle($|-(on|menu))").indexIn(icon) != -1) return "media-playlist-shuffle";
    if (QRegExp("about|info").indexIn(icon) != -1)            return "help-about";

    return {}; /** Se não estiver disponível, vai sem mesmo */
}


/** Usado para tirar o "\n" dos debugs */
void Utils::rm_nl(string &s) { for (uint p = s.find('\n'); p != (uint) string::npos; p = s.find('\n')) s.erase(p, 1); }


/** Alteração dos botões */
void Utils::changeIcon(QPushButton *btn, const QString &thm) {
    if (Utils::setIconTheme(def.definedTheme, thm) == nullptr)
        btn->setIcon(QIcon::fromTheme(defaultIcon(thm)));
    else btn->setIcon(QIcon(Utils::setIconTheme(def.definedTheme, thm)));
}


/** Alteração dos botões */
void Utils::changeIcon(QLabel *btn, const QString &thm) {
    if (Utils::setIconTheme(def.definedTheme, thm) == nullptr)
        btn->setPixmap(QPixmap(defaultIcon(thm)).scaled(36, 36, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else btn->setPixmap(QPixmap(Utils::setIconTheme(def.definedTheme, thm)).scaled(
            36, 36, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}


/** Definição dos ícones do menu */
void Utils::changeMenuIcon(QAction &btn, const QString &thm) {
    if (Utils::setIconTheme(def.definedTheme, thm) == nullptr)
        btn.setIcon(QIcon::fromTheme(defaultIcon(thm)));
    else btn.setIcon(QIcon(Utils::setIconTheme(def.definedTheme, thm)));
}


/** Definição dos ícones do menu */
void Utils::changeMenuIcon(QMenu *btn, const QString &thm) {
    if (Utils::setIconTheme(def.definedTheme, thm) == nullptr)
        btn->setIcon(QIcon::fromTheme(defaultIcon(thm)));
    else btn->setIcon(QIcon(Utils::setIconTheme(def.definedTheme, thm)));
}


/** Capturando o hash MD5 de um arquivo */
QString Utils::setHash(const QString &url) {
    QCryptographicHash hash(QCryptographicHash::Md5);
    QFile f(url);
    QFileInfo fi(url);

    int fsize = int(fi.size());
    int bsize = 262144; /** 256KiB */

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
        qDebug("%s(%sUtils%s)%s::%sCapturando MD5 Hash %s ...\033[0m",
               GRE, RED, GRE, RED, BLU, STR(hash.result().toHex()));
        return hash.result().toHex();
    } else return {};
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

    QStringList splitt = env.split(':');
    for (const auto &i : splitt) {
        QDir dir(i + "/usr/share/OMPlayer");
        if (dir.exists()) {
            qDebug("%s(%sUtils%s)%s::%sUsando XDG_DATA_DIRS: %s%s ...\033[0m",
                   GRE, RED, GRE, RED, HID, SHW, STR(i));
            def.definedXDG = i;
            return i;
        }
    }
    return {};
}


/** Retorno das dimensões de tela */
double Utils::aspectNum(Utils::AspectRatio option) {
    switch (option) {
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
QString Utils::aspectStr(Utils::AspectRatio option) {
    switch (option) {
        case Utils::AspectVideo:  return QObject::tr("Video");
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
        default: return QObject::tr("Unknown");
    }
}


/** Definindo o nome da tema */
QString Utils::setThemeName() {
    if(def.definedTheme.isEmpty()) def.definedTheme = JsonTools::stringJson("theme");
    return def.definedTheme;
}


/** Efeito fade para os diálogos */
void Utils::fadeDiag(QPropertyAnimation *animation, float x, float y) {
    animation->setDuration(FADE_VAL);
    animation->setStartValue(x);
    animation->setEndValue(y);
    animation->start();
}
