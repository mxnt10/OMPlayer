#include <QHeaderView>
#include <Utils>

#include "TreeView.hpp"


/**********************************************************************************************************************/


/** Construtor para definir as propriedades */
TreeView::TreeView(const QList<QTreeWidgetItem *>& item, QWidget *parent) : QTreeWidget(parent) {
    setStyleSheet(Utils::setStyle("infoview"));
    setHeaderHidden(true);
    setUniformRowHeights(true);
    setColumnCount(2);
    addTopLevelItems(item);

    /** Propriedades definidas para o redirecionamento */
    header()->setStretchLastSection(true);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    /** Prevenindo a exibição do scrool bar */
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}


/** Destrutor */
TreeView::~TreeView() = default;


/**********************************************************************************************************************/


/** Funções que retornam o tamanho das linhas do treeview */
int TreeView::ItemWith() { return header()->sectionSize(0) + header()->sectionSize(1); }
int TreeView::ItemHeight() { return visualRect(model()->index(0, 0, rootIndex())).height(); }
