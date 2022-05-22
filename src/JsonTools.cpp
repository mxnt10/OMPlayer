#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "JsonTools.h"

/** Renderização:
 * VideoRendererId_OpenGLWidget - openglwidget
 * VideoRendererId_GLWidget2    - qglwidget2
 * VideoRendererId_Direct2D     - direct2d
 * VideoRendererId_GDI          - gdi
 * VideoRendererId_XV           - xvideo
 * VideoRendererId_X11          - x11
 * VideoRendererId_Widget       - widget
 */

/**********************************************************************************************************************/


/** Função que retorna a estrutura que irá possuir o arquivo json original */
QString JsonTools::defaultJson() {
    return "{"
           "    \"renderer\": \"openglwidget\""
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
QString JsonTools::readJson(const QString &text) {
    QFile f(setJson());
    if (!f.open(QIODevice::ReadOnly)) verifySettings();
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject sett = d.object();
    QJsonValue item = sett[text];
    return item.toString();
}


/** */
void JsonTools::writeJson(const QString &text, const QString &type) {
    QFile f(setJson());
    if (!f.open(QIODevice::ReadOnly)) verifySettings();
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject sett = d.object();
}
