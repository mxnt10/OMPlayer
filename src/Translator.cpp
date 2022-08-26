#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QLocale>

#include "Translator.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Construtor para o tradutor */
Translator::Translator(QObject *parent) : QObject(parent) {
    translator = new QTranslator(this);
    QString lang = QLocale::system().bcp47Name();

    QString tr{QString(Utils::defaultDir() + "/i18n/OMPlayer_") + lang + ".qm"};
    QString ltr{Utils::getLocal() + "/i18n/OMPlayer_" + lang + ".qm"};

    if (QFileInfo::exists(tr)) local = tr;
    else if (QFileInfo::exists(ltr)) local = ltr;
}


/**********************************************************************************************************************/


/** Função que retorna se alguma tradução foi carregada */
bool Translator::load() {
    return translator->load(local);
}


/** Função para instalar a tradução */
void Translator::installTranslator() {
    qDebug("%s(%sTranslator%s)%s::%sArquivo de tradução carregado: %s ...\033[m", GRE, RED, GRE, RED, YEL,
           qUtf8Printable(local));
    QCoreApplication::installTranslator(translator);
}
