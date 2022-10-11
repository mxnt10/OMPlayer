#include <QCoreApplication>
#include <QFileInfo>
#include <QLocale>
#include <Utils>

#include "Translator.h"


/**********************************************************************************************************************/


/** Construtor para o tradutor */
Translator::Translator(QObject *parent) : QObject(parent) {
    translator = new QTranslator(this);
    QString lang = QLocale::system().bcp47Name();
    QStringList locals = Utils::setLocals();
    QString tr;

    for (int i = 0; i < 3; i++) {
        tr = locals[i] + "/i18n/OMPlayer_" + lang + ".qm";
        if (QFileInfo::exists(tr)) {
            local = tr;
            break;
        }
    }
}


/**********************************************************************************************************************/


/** Função que retorna se alguma tradução foi carregada */
bool Translator::load() { return translator->load(local); }


/** Função para instalar a tradução */
void Translator::installTranslator() {
    qDebug("%s(%sTranslator%s)%s::%sCarregando tradução: %s ...\033[m", GRE, RED, GRE, RED, YEL, STR(local));
    QCoreApplication::installTranslator(translator);
}
