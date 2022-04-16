#include <QPainter>
#include <QApplication>

#include "PlayListModel.h"
#include "PlayListDelegate.h"
#include "PlayListItem.h"


/**********************************************************************************************************************/


/** Definição de constantes */
static const int marginLeft = 5;
static const int marginTop = 4;
static const int width = 320;
static const int heightMax = 36;
static const int heightMin = 26;


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


    /** Itens selecionados */
    if (option.state & QStyle::State_Selected) {
        detail = true;
        selectedRows.append(index.row());
        painter->fillRect(QRect(0, 0, width, heightMax), QColor(0, 100, 200, 100));
    } else {
        selectedRows.removeAll(index.row());
    }


    /** Itens não selecionados */
    if (option.state & (QStyle::State_MouseOver)) {
        detail = true;
        auto *m = (PlayListModel *) index.model();
        if (m && highlightRow != index.row()) {
            highlightRow = index.row();
            m->updateLayout();
        }
        painter->fillRect(QRect(0, 0, width, heightMax), QColor(0, 100, 200, 30));
    }


    /** Definições de fonte */
    QFont ft;
    ft.setBold(detail);
    ft.setPixelSize(12);


    /** Definindo o texto do item da playlist */
    painter->setFont(ft);
    painter->translate(marginLeft, marginTop);
    painter->drawText(QRect(0, 0, width - 2 * marginLeft, heightMin - 2 * marginTop), pli.title());
    painter->translate(0, heightMin + marginTop);


    /** Definindo a descrição */
    if (detail) {
        painter->save();
        ft.setBold(false);
        painter->setFont(ft);
        painter->drawText(0, 0, pli.durationString() + QString::fromLatin1(" - ") + pli.format());
        painter->restore();
    }
    painter->restore();
}


/** Mapeador de eventos para o redirecionamento dos itens selecionados */
QSize PlayListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (!index.data().canConvert<PlayListItem>()) {
        return QStyledItemDelegate::sizeHint(option, index);
    }
    bool detail = option.state & (QStyle::State_Selected | QStyle::State_MouseOver);
    if (detail || highlightRow == index.row() || selectedRows.contains(index.row())) {
        return {width, heightMax};
    }
    return {width, heightMin};
}
