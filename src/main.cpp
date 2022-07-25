/**
 * OMPlayer - Open Multimedia player - Reprodutor multimídia desenvolvido em Qt5 e QtAV.
 *
 * Reprodutor multimídia desenvolvido em C++ usando como interface o Qt5.
 * Para reproduzir arquivos multimídia, o programa usa o framework QtAV,
 * que usa o ffmpeg para a manipulação de arquivos multimídia.
 *
 * Dependências:
 *      - Qt5
 *      - QtAV
 *      - libmediainfo
 *      - libzen
 *
 * Licença: GNU General Public License Version 3 (GLPv3)
 *
 * Mantenedor: Mauricio Ferrari
 * E-Mail: m10ferrari1200@gmail.com
 * Telegram: @maurixnovatrento
 *
 **********************************************************************************************************************/

#include <QDir>
#include <QLoggingCategory>
#include <QCommandLineParser>

#include <filesystem>
#include <SingleApplication>

#include "JsonTools.h"
#include "Player.h"
#include "Utils.h"

#define DEBUG true


/**********************************************************************************************************************/


int main(int argc, char *argv[]) {
    SingleApplication Player(argc, argv, true, SingleApplication::Mode::SecondaryNotification);
    std::filesystem::create_directory(QDir::homePath().toStdString() + "/.config/OMPlayer");
    JsonTools::verifySettings();

    qDebug("%s(%sSingleApplication%s)%s::%sInformações da instância atual:"
           "\n                     - PID Primário: %lli"
           "\n                     - Usuário primário: %s"
           "\n                     - Usuário atual: %s"
           "\n                     - ID da instância: %i"
           "\033[m", GRE, RED, GRE, RED, BLU,
           Player.primaryPid(),
           qUtf8Printable(Player.primaryUser()),
           qUtf8Printable(Player.currentUser()),
           Player.instanceId());

    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "Simplify"
    #pragma ide diagnostic ignored "UnreachableCode"

    QLoggingCategory::setFilterRules("*.error=false\n*.warning=false\n"
                                     "*.critical=false\n*.info=false");

    if (!DEBUG) /** Instrução apenas para depuração */
        QLoggingCategory::setFilterRules("*=false");

    #pragma clang diagnostic pop

    /** Propriedades do Programa */
    QCoreApplication::setApplicationName("OMPlayer");
    QCoreApplication::setApplicationVersion(VERSION);
    QGuiApplication::setApplicationDisplayName(QCoreApplication::applicationName());

    /** Instruções que permite a passagem de argumentos para o reprodutor */
    QCommandLineParser parser;
    parser.setApplicationDescription("Open Multimedia Player");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("url_files", QApplication::tr("Open multimedia files."));
    parser.process(Player);

    /** Define a interface do programa, envia os argumentos para o reprodutor */
    OMPlayer player;
    if (!parser.positionalArguments().isEmpty()) {
        player.openMedia(parser.positionalArguments());
    }

    /** Verificando instâncias abertas e impedindo novas instâncias */
    if( Player.isSecondary() ) {
        Player.sendMessage("OK");
        qDebug("%s(%sSingleApplication%s)%s::%sInstância secundária, fechando!\033[m", GRE, RED, GRE, RED, BLU);
        return 0;
    } else {
        QObject::connect(&Player, &SingleApplication::instanceStarted, [&player]() {
            qDebug("%s(%sSingleApplication%s)%s::%sAberto outra instância ...\033[m", GRE, RED, GRE, RED, BLU);
            player.hide();
            player.show();
            player.activateWindow();
        });
        QObject::connect(&Player, &SingleApplication::receivedMessage, [&player]() {
            qDebug("%s(%sSingleApplication%s)%s::%sRecarregando playlist ...\033[m", GRE, RED, GRE, RED, BLU);
            player.onLoad();
        });
    }

    player.show();
    return QApplication::exec();
}
