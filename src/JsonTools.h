#ifndef OMPLAYER_JSONTOOLS_H
#define OMPLAYER_JSONTOOLS_H

#define NONE  (-9999999)
#define TRUE  "00000000"
#define FALSE "00000001"

class JsonTools {
public:
    static void verifySettings();
    static QString stringJson(const QString &text);
    static int intJson(const QString &text);
    static double floatJson(const QString &text);
    static bool boolJson(const QString &text);
    static void writeJson(const QString &key, const QString &value = nullptr,
                          int valuei = NONE, double valuef = NONE);

private:
    static QJsonValue returnJson(const QString &text);
    static QString defaultJson();
    static QString localJson();
};

#endif //OMPLAYER_JSONTOOLS_H
