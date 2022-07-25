#include <QTime>
#include <QDebug>
#include <QDataStream>

#include "PlayListItem.h"


/**********************************************************************************************************************/


/** Construtor */
PlayListItem::PlayListItem() = default;


/** Cria um fluxo de dados binário para a playlist */
QDataStream &operator>>(QDataStream & s, PlayListItem & p) {
    qint64 duration;
    QString url, title, format;
    s >> url >> title >> duration >> format;
    p.setTitle(title);
    p.setUrl(url);
    p.setDuration(duration);
    p.setFormat(format);
    return s;
}


/** Adiciona itens ao fluxo de dados binário da playlist */
QDataStream &operator<<(QDataStream & s, const PlayListItem &p) {
    s << p.url() << p.title() << p.duration() << p.format();
    return s;
}


/**********************************************************************************************************************/


/** Define o título do arquivo multimídia */
void PlayListItem::setTitle(const QString &title) {
    ftitle = title;
}


/** Retorna o título do arquivo multimídia */
QString PlayListItem::title() const {
    return ftitle;
}


/** Define a localização do arquivo multimídia */
void PlayListItem::setUrl(const QString &url) {
    furl = url;
}


/** Retorna a localização do arquivo multimídia */
QString PlayListItem::url() const {
    return furl;
}


/** Retorna a duração do arquivo multimídia */
qint64 PlayListItem::duration() const {
    return fduration;
}


/** Retorna o formato do arquivo multimídia */
QString PlayListItem::format() const {
    return formatstr;
}


/** Setando a duração do arquivo multimídia */
void PlayListItem::setDuration(qint64 ms) {
    fduration = ms;
    durationstr = QTime(0, 0, 0, 0).addMSecs(int(ms)).toString(QString::fromLatin1("HH:mm:ss"));
}


/** Setando o formato do arquivo multimídia */
void PlayListItem::setFormat(const QString &format) {
    formatstr = format;
}


/** Retorna a string da duração do arquivo multimídia */
QString PlayListItem::durationString() const {
    return durationstr;
}


/** Operação para localizar a url do arquivo multimídia */
bool PlayListItem::operator==(const PlayListItem &other) const {
    return url() == other.url();
}
