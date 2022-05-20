#ifndef OMPLAYER_JSONTOOLS_H
#define OMPLAYER_JSONTOOLS_H

#define BOOL 0

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE


class JsonTools {
public:

    static void verifySettings();
    static QString readJson(const QString &text, int type = BOOL);
    static QString writeJson();

private:
    static QString defaultJson();
    static QString setJson();

};

#endif //OMPLAYER_JSONTOOLS_H
