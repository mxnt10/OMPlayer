#ifndef MESSAGERECEIVER_H
#define MESSAGERECEIVER_H

#include <QObject>


/**
 * Classe QObject para receber a mensagem.
 **********************************************************************************************************************/


class MessageReceiver : public QObject {
Q_OBJECT
public:
    explicit MessageReceiver(QObject *parent = nullptr);

public slots:
    static void receivedMessage( int instanceId, const QByteArray& message);

};

#endif // MESSAGERECEIVER_H
