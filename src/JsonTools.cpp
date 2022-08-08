#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "JsonTools.h"


/**********************************************************************************************************************/


/** Função que retorna a estrutura que irá possuir o arquivo json original */
QString JsonTools::defaultJson() {
    return "{"
           "    \"maximized\": false,"
           "    \"on_replay\": false,"
           "    \"on_shuffle\": false,"
           "    \"pls_listsize\": 280,"
           "    \"pls_size\": 295,"
           "    \"renderer\": \"OpenGL\","
           "    \"theme\": \"circle\","
           "    \"volume\": 1"
           "}";
}


/** Retorna a localização do arquivo de configuração do programa */
QString JsonTools::localJson() {
    return QDir::homePath() + "/.config/OMPlayer/settings.json";
}


/** Função que verifica a existência do arquivo de configurações do programa em json. Se não encontrado,
 * um arquivo de configuração será criado. */
void JsonTools::settingsJson() {
    QFile f(localJson());
    if (!f.open(QIODevice::ReadOnly)) {
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
        QJsonDocument d = QJsonDocument::fromJson(defaultJson().toUtf8());
        f.write(d.toJson(QJsonDocument::Indented));
    }
    f.close();
}


/** Função para ler um determinado item de um arquivo json */
QJsonValue JsonTools::valueJson(const QString &text) {
    QFile f(localJson());
    if (!f.open(QIODevice::ReadOnly)) settingsJson();
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    return d.object().value(text);
}


/** Função para gravar um determinado item em um arquivo json */
void JsonTools::valueJson(const QString &key, const QJsonValue &value) {
    QFile f(localJson());
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) settingsJson();
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    f.close();
    QJsonObject sett = d.object();
    sett.insert(key, value);
    d.setObject(sett);
    f.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    f.write(d.toJson());
    f.close();
}


/** Função que recebe o item de um arquivo json e converte em string */
QString JsonTools::stringJson(const QString &text) {
    return valueJson(text).toString();
}


/** Função que grava uma string em um arquivo json */
void JsonTools::stringJson(const QString &key, const QString &value) {
    valueJson(key, QJsonValue(value));
}


/** Função que recebe o item de um arquivo json e converte em inteiro */
int JsonTools::intJson(const QString &text) {
    return valueJson(text).toInt();
}


/** Função que grava um inteiro em um arquivo json */
void JsonTools::intJson(const QString &key, int value) {
    valueJson(key, QJsonValue(value));
}


/** Função que recebe o item de um arquivo json e converte em double */
double JsonTools::floatJson(const QString &text) {
    return valueJson(text).toDouble();
}


/** Função que grava um float/double em um arquivo json */
void JsonTools::floatJson(const QString &key, float value) {
    valueJson(key, QJsonValue(value));
}


/** Função que recebe o item de um arquivo json e converte em bool */
bool JsonTools::boolJson(const QString &text) {
    return valueJson(text).toBool();
}


/** Função que grava um valor booleano em um arquivo json */
void JsonTools::boolJson(const QString &key, bool value) {
    valueJson(key, QJsonValue(value));
}
