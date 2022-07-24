#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "JsonTools.h"


/**********************************************************************************************************************/


/** Função que retorna a estrutura que irá possuir o arquivo json original */
QString JsonTools::defaultJson() {
    return "{"
           "    \"renderer\": \"OpenGL\","
           "    \"theme\": \"circle\","
           "    \"volume\": 1.0"
           "}";
}


/** Retorna a localização do arquivo de configuração do programa */
QString JsonTools::localJson() {
    return QDir::homePath() + "/.config/OMPlayer/settings.json";
}


/** Função que verifica a existência do arquivo de configurações do programa em json. Se não encontrado,
 * um arquivo de configuração será criado. */
void JsonTools::verifySettings() {
    QFile f(localJson());
    if (!f.open(QIODevice::ReadOnly)) {
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
        QJsonDocument d = QJsonDocument::fromJson(defaultJson().toUtf8());
        f.write(d.toJson(QJsonDocument::Indented));
    }
    f.close();
}


/** Função para ler um determinado item de um arquivo json */
QJsonValue JsonTools::returnJson(const QString &text) {
    QFile f(localJson());
    if (!f.open(QIODevice::ReadOnly)) verifySettings();
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject sett = d.object();
    return sett[text];
}


/** Função que recebe o item de um arquivo json e converte em string */
QString JsonTools::stringJson(const QString &text) {
    return returnJson(text).toString();
}


/** Função que recebe o item de um arquivo json e converte em inteiro */
int JsonTools::intJson(const QString &text) {
    return returnJson(text).toInt();
}


/** Função que recebe o item de um arquivo json e converte em double */
double JsonTools::floatJson(const QString &text) {
    return returnJson(text).toDouble();
}


/** Função que recebe o item de um arquivo json e converte em double */
bool JsonTools::boolJson(const QString &text) {
    return returnJson(text).toBool();
}


/** Função para modificar um determinado item de um arquivo json */
void JsonTools::writeJson(const QString &key, const QString &valuestr, int valuei, double valuef) {
    QFile f(localJson());
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) verifySettings();
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    f.close();
    QJsonObject sett = d.object();
    if (QString::compare(valuestr, TRUE) == 0)  sett.insert(key, true);
    else if (QString::compare(valuestr, FALSE) == 0) sett.insert(key, false);
    else if (!valuestr.isEmpty()) sett.insert(key, valuestr);
    else if (valuei != NONE) sett.insert(key, valuei);
    else if (valuef != NONE) sett.insert(key, valuef);
    d.setObject(sett);
    f.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    f.write(d.toJson());
    f.close();
}
