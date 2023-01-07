#ifndef OMPLAYER_LISTVIEW_H
#define OMPLAYER_LISTVIEW_H

#include <QListView>

class ListView : public QListView {
Q_OBJECT

public:
    explicit ListView(QWidget *parent = nullptr);
    ~ListView() override;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

Q_SIGNALS:
    void clickedRight(QModelIndex index);

};

#endif //OMPLAYER_LISTVIEW_H
