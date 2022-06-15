#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <QVariant>


class PlayListItem {
public:
    PlayListItem();
    [[nodiscard]] QString title() const;
    [[nodiscard]] QString url() const;
    [[nodiscard]] qint64 duration() const;
    [[nodiscard]] QString durationString() const;
    [[nodiscard]] QString format() const;
    void setTitle(const QString &title);
    void setUrl(const QString &url);
    void setFormat(const QString &format);
    void setDuration(qint64 ms);
    bool operator==(const PlayListItem &other) const;

private:
    QString ftitle;
    QString furl;
    QString formatstr;
    QString durationstr;
    qint64 fduration;
};

Q_DECLARE_METATYPE(PlayListItem);

QDataStream &operator>>(QDataStream & s, PlayListItem & p);
QDataStream &operator<<(QDataStream & s, const PlayListItem &p);

#endif // PLAYLISTITEM_H
