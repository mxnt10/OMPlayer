#include <QString>

#include "Defines.h"


/**********************************************************************************************************************/


/** Informações adicionais */
QString About::getTextMaintainer() {
    return "Maintainer: " + QString::fromStdString(MAINTAINER) + "\n" + \
    "E-Mail: " + QString::fromStdString(EMAIL) + "\n\n" + \
    "License: " + QString::fromStdString(LICENSE);
}


/** Descrição do programa */
QString About::getDescription() {
    return QString::fromStdString("Multimedia player developed in C++ using Qt5 as an interface. ") + \
    QString::fromStdString("To play multimedia files, the program uses the QtAV framework, ") + "\n" + \
    QString::fromStdString("which uses ffmpeg for handling multimedia files.");
}
