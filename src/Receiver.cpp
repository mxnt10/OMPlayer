#include <QDebug>
#include <QFile>
#include <QDir>

#include "Defines.h"
#include "Receiver.h"


/**********************************************************************************************************************/


MessageReceiver::MessageReceiver(QObject *parent) : QObject(parent) {}


void MessageReceiver::receivedMessage(int instanceId, const QByteArray& message) {
    qDebug("%s(%sDEBUG%s):%s %s: %i!\033[m", GRE, RED, GRE, BLU,
           qUtf8Printable(QString::fromLocal8Bit(message)), instanceId);
}
