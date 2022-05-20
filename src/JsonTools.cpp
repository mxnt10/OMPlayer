#include <QDir>
#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>

#include "JsonTools.h"


/**********************************************************************************************************************/


/** Função que retorna a estrutura que irá possuir o arquivo json original */
QString JsonTools::defaultJson() {
    return "{"
           "    \"openglwidget\": false,"
           "    \"qglwidget2\": true,"
           "    \"widget\": false,"
           "    \"xvideo\": false,"
           "    \"x11\": false"
           "}";
}


/** Retorna a localização do arquivo de configuração do programa */
QString JsonTools::setJson() {
    return QDir::homePath() + "/.config/OMPlayer/settings.json";
}


/** Função que verifica a existência do arquivo de configurações do programa em json. Se não encontrado,
 * um arquivo de configuração será criado. */
void JsonTools::verifySettings() {
    QFile f(setJson());
    if (!f.open(QIODevice::ReadOnly)) {
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
        QJsonDocument d = QJsonDocument::fromJson(defaultJson().toUtf8());
        f.write(d.toJson(QJsonDocument::Indented));
    }
    f.close();
}


/** */
QString JsonTools::readJson(const QString &text, int type) {
    QFile f(setJson());
    if (!f.open(QIODevice::ReadOnly)) verifySettings();
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject sett = d.object();
    QJsonValue item = sett[text];
    if (type == BOOL)
        return QVariant(item.toBool()).toString();
    return nullptr;
}


/** */
QString JsonTools::writeJson() {
    QFile f(setJson());
    if (!f.open(QIODevice::ReadOnly)) verifySettings();
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject sett = d.object();
    return {};
}

//QString s = "true";
//bool bInUse = QVariant(s).toBool();
