#include <QApplication>
#include <QPainter>

#include "PlayListModel.h"
#include "PlayListDelegate.h"
#include "PlayListItem.h"

#define margin 5
#define height 28


/**********************************************************************************************************************/


/** Construtor */
PlayListDelegate::PlayListDelegate(QObject *parent): QStyledItemDelegate(parent) {}


/** Destrutor */
PlayListDelegate::~PlayListDelegate() = default;


/**********************************************************************************************************************/


/** Redefinindo tamanho */
void PlayListDelegate::resetWith() {
    width = 260;
}


/** Definindo um tamanho específico */
void PlayListDelegate::setWith(int w) {
    width = w;
}


/**********************************************************************************************************************/


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
    if (pli.title().size() * 8 > width) width = pli.title().size() * 8;
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
        auto *m = (PlayListModel *) index.model();
        if (m && highlightRow != index.row()) {
            highlightRow = index.row();
            m->updateLayout();
        }
        if (option.state & (QStyle::State_MouseOver)) {
            if (*QApplication::overrideCursor() != QCursor(Qt::ArrowCursor))
                QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));

            detail = true;
            painter->fillRect(QRect(0, 0, width, height), QColor(0, 100, 200, 100));
        }
        if (index.row() % 2 == 0)
            painter->fillRect(QRect(0, 0, width, height), QColor(0, 100, 200, 30));
    }


    /** Definindo os itens da playlist */
    QFont ft;
    ft.setBold(detail);
    ft.setPixelSize(11);
    painter->setFont(ft);
    painter->translate(margin, margin);
    painter->drawText(QRect(0, -5, width - 2 * margin, height - 2 * margin),
                      QString::number(index.row() + 1) + ". " + pli.title());
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
    if (!index.data().canConvert<PlayListItem>()) return QStyledItemDelegate::sizeHint(option, index);
    return {width, height};
}
