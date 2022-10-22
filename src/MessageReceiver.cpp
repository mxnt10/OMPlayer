#include <Utils>
#include "MessageReceiver.h"


/**********************************************************************************************************************/


/** Construtor */
MessageReceiver::MessageReceiver(QObject *parent) : QObject(parent) {}


/**********************************************************************************************************************/


/** Função que recebe os parâmetros da instâncias secundárias */
void MessageReceiver::receivedMessage(int id, const QByteArray& message) {
    qDebug("%s(%sMessageReceiver%s)%s::%sArgumentos recebidos da instância %i ...\033[m", GRE, RED, GRE, RED, BLU, id);
    emit parms(QString::fromUtf8(message).split('\n'));
}
