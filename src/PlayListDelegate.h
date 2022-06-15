#ifndef PLAYLISTDELEGATE_H
#define PLAYLISTDELEGATE_H

#include <QStyledItemDelegate>


class PlayListDelegate : public QStyledItemDelegate {
Q_OBJECT
public:
    explicit PlayListDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    mutable int highlightRow;
    mutable QList<int> selectedRows;
};

#endif // PLAYLISTDELEGATE_H
