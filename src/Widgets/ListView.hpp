#ifndef OMPLAYER_LISTVIEW_HPP
#define OMPLAYER_LISTVIEW_HPP

#include <QListView>
#include <QStyledItemDelegate>

class ListView : public QListView {
Q_OBJECT

public:
    explicit ListView(QStyledItemDelegate *delegate, QWidget *parent = nullptr);
    ~ListView() override;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

Q_SIGNALS:
    void clickedRight(QModelIndex index);

};

#endif //OMPLAYER_LISTVIEW_HPP