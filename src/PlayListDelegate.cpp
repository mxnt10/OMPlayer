#include <QPainter>
#include <QApplication>

#include "PlayListModel.h"
#include "PlayListDelegate.h"
#include "PlayListItem.h"


/**********************************************************************************************************************/


/** Definição de constantes */
static const int margin = 5;
static const int width = 320;
static const int height = 28;


/**********************************************************************************************************************/


/** Construtor */
PlayListDelegate::PlayListDelegate(QObject *parent): QStyledItemDelegate(parent) {
    highlightRow = -1;
}


/** O tamanho da fonte e o formato dos itens exibidos pela playlist são definidos aqui */
void PlayListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (!index.data().canConvert<PlayListItem>()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }


    /** Definindo as propriedades */
    painter->save();
    painter->translate(option.rect.topLeft());
    painter->setRenderHint(QPainter::Antialiasing, true);
    auto pli = qvariant_cast<PlayListItem>(index.data(Qt::DisplayRole));
    bool detail = false;


    /** Itens selecionados - Clicados */
    if (option.state & QStyle::State_Selected) {
        detail = true;
        selectedRows.append(index.row());
        painter->fillRect(QRect(0, 0, width, height), QColor(0, 100, 200, 150));
    } else {
        selectedRows.removeAll(index.row());
    }


    /** Itens com o mouse posicionado e destaque das linhas uma das outras */
    if (option.state & (QStyle::State_MouseOver) || index.row() % 2 == 0 ) {
        detail = true;
        auto *m = (PlayListModel *) index.model();
        if (m && highlightRow != index.row()) {
            highlightRow = index.row();
            m->updateLayout();
        }
        if (option.state & (QStyle::State_MouseOver))
            painter->fillRect(QRect(0, 0, width, height), QColor(0, 100, 200, 100));
        if (index.row() % 2 == 0)
            painter->fillRect(QRect(0, 0, width, height), QColor(0, 100, 200, 30));
    }


    /** Definindo os itens da playlist */
    QFont ft;
    ft.setBold(detail);
    ft.setPixelSize(11);
    painter->setFont(ft);
    painter->translate(margin, margin);
    painter->drawText(QRect(0, -5, width - 2 * margin, height - 2 * margin), pli.title());
    painter->translate(0, height + margin - 1);


    /** Definindo a descrição */
    if (detail && option.state & (QStyle::State_MouseOver | QStyle::State_Selected) && !pli.format().isEmpty()) {
        painter->save();
        ft.setBold(false);
        painter->setFont(ft);
        painter->drawText(0, -12, pli.durationString() + QString::fromLatin1(" - ") + pli.format());
        painter->restore();
    }
    painter->restore();
}


/** Mapeador de eventos para o redirecionamento das linhas dos itens da playlist */
QSize PlayListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (!index.data().canConvert<PlayListItem>()) {
        return QStyledItemDelegate::sizeHint(option, index);
    }
    return {width, height};
}
