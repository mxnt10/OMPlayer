#ifndef OMPLAYER_TRANSLATOR_H
#define OMPLAYER_TRANSLATOR_H

#include <QTranslator>

class Translator : public QObject {
Q_OBJECT

public:
    explicit Translator(QObject *parent = nullptr);
    void installTranslator();
    bool load();

private:
    QTranslator *translator;
    QString local{};
};

#endif //OMPLAYER_TRANSLATOR_H
