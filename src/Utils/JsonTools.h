#ifndef OMPLAYER_JSONTOOLS_H
#define OMPLAYER_JSONTOOLS_H

#include <QJsonValue>

class JsonTools {
public:
    static void settingsJson();
    static QString stringJson(const QString &key);
    static void stringJson(const QString &key, const QString &value);
    static int intJson(const QString &key);
    static void intJson(const QString &key, int value);
    static double floatJson(const QString &key);
    static void floatJson(const QString &key, float value);
    static bool boolJson(const QString &key);
    static void boolJson(const QString &key, bool value);

private:
    static QString localJson();
    static QString defaultJson();
    static QJsonValue valueJson(const QString &key);
    static void valueJson(const QString &key, const QJsonValue &value);
};

#endif //OMPLAYER_JSONTOOLS_H
