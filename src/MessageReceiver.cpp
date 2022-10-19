#include <QDebug>
#include "MessageReceiver.h"

MessageReceiver::MessageReceiver(QObject *parent) : QObject(parent) {}

void MessageReceiver::receivedMessage(int instanceId, const QByteArray& message) {
    qDebug() << "Received message from instance: " << instanceId;
    emit parms(QString::fromUtf8(message).split('\n'));
}
