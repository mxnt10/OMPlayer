#include <QApplication>
#include <QString>
#include "Defines.h"


/**********************************************************************************************************************/


/** Informações adicionais */
QString About::getTextMaintainer() {
    return QApplication::tr("Maintainer") + ": " + QString::fromStdString(MAINTAINER) + "\n" + \
    QApplication::tr("E-Mail") + ": " + QString::fromStdString(EMAIL) + "\n\n" + \
    QApplication::tr("License") + ": " + QString::fromStdString(LICENSE);
}


/** Descrição do programa */
QString About::getDescription() {
    return QApplication::tr("Multimedia player developed in C++ using Qt5 as an interface. ") + \
    QApplication::tr("To play multimedia files, the program uses the QtAV framework, ") + "\n" + \
    QApplication::tr("which uses ffmpeg for handling multimedia files.");
}
