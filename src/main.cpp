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
 *      - pyUserInput
 *
 * Licença: GNU General Public License Version 3 (GLPv3)
 *
 * Mantenedor: Mauricio Ferrari
 * E-Mail: m10ferrari1200@gmail.com
 * Telegram: @maurixnovatrento
 *
 *
 **********************************************************************************************************************/


#include <QApplication>
#include <QCommandLineParser>
#include <QLoggingCategory>

#include "Player.h"
#include "Defines.h"


/**
 * Início do Programa.
 **********************************************************************************************************************/


int main(int argc, char *argv[]) {
    QApplication OMPlayer(argc, argv);

    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "Simplify"
    #pragma ide diagnostic ignored "UnreachableCode"

    if (!DEBUG) /** Instrução apenas para depuração */
        QLoggingCategory::setFilterRules("*.debug=false");

    #pragma clang diagnostic pop


    /** Propriedades do Programa */
    QCoreApplication::setApplicationName("OMPlayer");
    QCoreApplication::setApplicationVersion(VERSION);
    QGuiApplication::setApplicationDisplayName(QCoreApplication::applicationName());


    /** Instruções que permite a passagem de argumentos para o reprodutor */
    QCommandLineParser parser;
    parser.setApplicationDescription(PRG_NAME);
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("url_files", QApplication::tr("Open multimedia files."));
    parser.process(OMPlayer);

//    QCommandLineOption targetDirectoryOption(QStringList() << "t" << "target-directory",
//                                             QCoreApplication::translate("main", "Copy all source files into <directory>."),
//                                             QCoreApplication::translate("main", "directory"));
//    parser.addOption(targetDirectoryOption);

    /** Define a interface do programa, envia os argumentos para o reprodutor e inicia a interface */
    VideoPlayer player;
    if (!parser.positionalArguments().isEmpty()) {
        player.openMedia(parser.positionalArguments());
    }
    player.show();
    return QApplication::exec();
}
