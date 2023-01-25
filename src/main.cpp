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
 *      - libnotify
 *
 * Dependências incluídas:
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
#include <QCommandLineParser>
#include <QLoggingCategory>
#include <SingleApplication>
#include <Utils>

#include "MessageReceiver.hpp"
#include "Player.h"
#include "Translator.hpp"

#define DEBUG true


/**********************************************************************************************************************/


int main(int argc, char *argv[]) {
    SingleApplication Player(argc, argv, true, SingleApplication::Mode::SecondaryNotification);
    MessageReceiver msg;


    /**************************************** Instrução apenas para depuração *************************************/
    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "Simplify"
    #pragma ide diagnostic ignored "UnreachableCode"

    QLoggingCategory::setFilterRules("*.error=false\n*.warning=false\n*.critical=false\n*.info=false");
    if (!DEBUG) QLoggingCategory::setFilterRules("*=false");

    #pragma clang diagnostic pop
    /**************************************************************************************************************/


    /** Debug geral da aplicação */
    qDebug("%s(%sSingleApplication%s)%s::%sInformações da instância atual:"
           "\n                     - PID Primário: %lli"
           "\n                     - Usuário primário: %s"
           "\n                     - Usuário atual: %s"
           "\n                     - ID da instância: %i"
           "\033[m", GRE, RED, GRE, RED, BLU,
           Player.primaryPid(),
           STR(Player.primaryUser()),
           STR(Player.currentUser()),
           Player.instanceId());

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

    /** Verificação de instâncias secundárias para a passagem de parâmetros para a primeira */
    if (Player.isSecondary()) {
        Player.sendMessage(parser.positionalArguments().join('\n').toUtf8());
        qDebug("%s(%sSingleApplication%s)%s::%sInstância secundária, fechando!\033[m", GRE, RED, GRE, RED, RDL);
        return 0;
    }

    /** Verificando diretório padrão */
    Utils::initUtils();
    QDir dir(QDir::homePath() + "/.config/OMPlayer");
    if (!dir.exists()) dir.mkpath(".");
    JsonTools::settingsJson();

    /** Instalando tradução e construindo a interface */
    Translator translator;
    if (translator.load()) translator.installTranslator();

    OMPlayer player;
    if (!parser.positionalArguments().isEmpty()) player.openMedia(parser.positionalArguments());

    QObject::connect(&Player, &SingleApplication::instanceStarted, [&player]() {
        qDebug("%s(%sSingleApplication%s)%s::%sAberto outra instância ...\033[m", GRE, RED, GRE, RED, BLU);
        auto eFlags = player.windowFlags();
        player.setWindowFlags(eFlags | Qt::WindowStaysOnTopHint);
        player.show();
        player.setWindowFlags(eFlags);
        player.show();
        player.raise();
        player.activateWindow();
    });

    /** Pegando os argumentos das instâncias secundárias */
    QObject::connect(&Player, &SingleApplication::receivedMessage, &msg, &MessageReceiver::receivedMessage);
    QObject::connect(&msg, &MessageReceiver::parms, &player, &OMPlayer::openMedia);

    if (JsonTools::boolJson("maximized")) player.showMaximized();
    else player.show();
    return QApplication::exec();
}
