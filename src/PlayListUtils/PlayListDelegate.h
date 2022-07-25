#ifndef PLAYLISTDELEGATE_H
#define PLAYLISTDELEGATE_H

#include <QStyledItemDelegate>

class PlayListDelegate : public QStyledItemDelegate {
Q_OBJECT
public:
    explicit PlayListDelegate(QObject *parent = nullptr);
    ~PlayListDelegate() override;
    void resetWith();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    mutable int highlightRow{-1};
    mutable QList<int> selectedRows;
    mutable int width{260};
};

#endif // PLAYLISTDELEGATE_H
