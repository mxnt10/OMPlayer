#ifndef OMPLAYER_JSONTOOLS_H
#define OMPLAYER_JSONTOOLS_H


/**
 * Classe JsonTools, para manipular arquivos json.
 **********************************************************************************************************************/


class JsonTools {
public:
    static void verifySettings();
    static QString readJson(const QString &text);
    static void writeJson(const QString &text, const QString &type);

private:
    static QString defaultJson();
    static QString setJson();
};

#endif //OMPLAYER_JSONTOOLS_H
