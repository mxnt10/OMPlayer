#ifndef OMPLAYER_PLAYLISTDELEGATE_HPP
#define OMPLAYER_PLAYLISTDELEGATE_HPP

#include <QStyledItemDelegate>

class PlayListDelegate : public QStyledItemDelegate {
Q_OBJECT

public:
    explicit PlayListDelegate(QObject *parent = nullptr);
    ~PlayListDelegate() override;
    void setWith(int w) { width = w; };

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    mutable int highlightRow{-1};
    mutable QList<int> selectedRows;
    mutable int width{260};
};

#endif // OMPLAYER_PLAYLISTDELEGATE_HPP
