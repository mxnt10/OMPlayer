#ifndef OMPLAYER_TREEVIEW_HPP
#define OMPLAYER_TREEVIEW_HPP

#include <QTreeWidget>

class TreeView : public QTreeWidget {
Q_OBJECT

public:
    explicit TreeView(const QList<QTreeWidgetItem*>& item, QWidget *parent = nullptr);
    ~TreeView() override;
    int ItemHeight();
    int ItemWith();
};

#endif //OMPLAYER_TREEVIEW_HPP
