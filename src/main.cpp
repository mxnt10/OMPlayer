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
 *
 **********************************************************************************************************************/


#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QLoggingCategory>

#include <filesystem>
#include <SingleApplication>

#include "Defines.h"
#include "JsonTools.h"
#include "Player.h"

using std::filesystem::create_directory;

//{
//        { "opengl", VideoRendererId_OpenGLWidget },
//        { "gl", VideoRendererId_GLWidget2 },
//        { "d2d", VideoRendererId_Direct2D },
//        { "gdi", VideoRendererId_GDI },
//        { "xv", VideoRendererId_XV },
//        { "x11", VideoRendererId_X11 },
//        { "qt", VideoRendererId_Widget },
//        { nullptr, 0 }
//};

/**
 * Início do Programa.
 **********************************************************************************************************************/


int main(int argc, char *argv[]) {
    SingleApplication OMPlayer(argc, argv, true);
    create_directory(QDir::homePath().toStdString() + "/.config/OMPlayer");
    JsonTools::verifySettings();


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
    parser.setApplicationDescription(PRG_NAME);
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("url_files", QApplication::tr("Open multimedia files."));
    parser.process(OMPlayer);

    /** Define a interface do programa, envia os argumentos para o reprodutor*/
    VideoPlayer player;
    if (!parser.positionalArguments().isEmpty()) {
        player.openMedia(parser.positionalArguments());
    }

    /** Verificando instâncias abertas e impedindo novas instâncias */
    if( OMPlayer.isSecondary() ) {
        OMPlayer.sendMessage("OK");
        return 0;
    } else {
        QObject::connect(&OMPlayer, &SingleApplication::receivedMessage, [&player]() {
            qDebug("%s(%sDEBUG%s):%s Aberto outra instância ...\033[m", GRE, RED, GRE, BLU);
            player.hide();
            player.show();
            player.activateWindow();
            player.onLoad();
        });
    }

    player.show();
    return QApplication::exec();
}
