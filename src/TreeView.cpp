#include <QHeaderView>

#include "TreeView.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Construtor para definir as propriedades */
TreeView::TreeView(const QList<QTreeWidgetItem *>& item, QWidget *parent) : QTreeWidget(parent) {
    setStyleSheet(Utils::setStyle("infoview"));
    setHeaderHidden(true);
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


/** Retorna o tamanho das linhas do treeview */
int TreeView::size() { return header()->sectionSize(0) + header()->sectionSize(1); }
