#ifndef OMPLAYER_TREEVIEW_H
#define OMPLAYER_TREEVIEW_H

#include <QTreeWidget>

class TreeView : public QTreeWidget {
Q_OBJECT
public:
    explicit TreeView(const QList<QTreeWidgetItem*>& item, QWidget *parent = nullptr);
    ~TreeView() override;
    int size();
};

#endif //OMPLAYER_TREEVIEW_H
