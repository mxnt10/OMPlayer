#ifndef OMPLAYER_MESSAGERECEIVER_H
#define OMPLAYER_MESSAGERECEIVER_H

#include <QObject>

class MessageReceiver : public QObject {
Q_OBJECT

public:
    explicit MessageReceiver(QObject *parent = nullptr);

public slots:
    void receivedMessage( int instanceId, const QByteArray& message );

Q_SIGNALS:
    void parms(const QStringList &lst);
};

#endif // OMPLAYER_MESSAGERECEIVER_H
