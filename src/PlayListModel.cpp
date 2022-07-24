#include <QVector>

#include "PlayListModel.h"
#include "PlayListItem.h"


/**********************************************************************************************************************/


/** Construtor do modelo da playlist */
PlayListModel::PlayListModel(QObject *parent) : QAbstractListModel(parent) {}


/** Destrutor */
PlayListModel::~PlayListModel() = default;


/**********************************************************************************************************************/


/** Retorna os itens da playlist */
QList<PlayListItem> PlayListModel::items() const {
    return mediaItems;
}


/** Retorna o número de itens da playlist */
int PlayListModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return mediaItems.size();
}


/** Usado para emissão na função onAboutToPlay */
QVariant PlayListModel::data(const QModelIndex &index, int role) const {
    QVariant var;
    if (index.row() < 0 || index.row() >= mediaItems.size())
        return var;

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        var.setValue(mediaItems.at(index.row()));

    return var;
}


/** Função auxiliar para buscar itens da playlist */
bool PlayListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.row() >= 0 && index.row() < mediaItems.size() && (role == Qt::EditRole || role == Qt::DisplayRole)) {
        mediaItems.replace(index.row(), value.value<PlayListItem>());
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}


/** Função para inserir itens na playlist */
bool PlayListModel::insertRows(int row, int count, const QModelIndex &parent) {
    if (count < 1 || row < 0 || row > rowCount(parent))
        return false;
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int r = 0; r < count; ++r)
        mediaItems.insert(row, PlayListItem());
    endInsertRows();
    return true;
}


/** Função para remover itens da playlist */
bool PlayListModel::removeRows(int row, int count, const QModelIndex &parent) {
    if (count <= 0 || row < 0 || (row + count) > rowCount(parent))
        return false;
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int r = 0; r < count; ++r)
        mediaItems.removeAt(row);
    endRemoveRows();
    return true;
}


/** Atualização do layout da playlist */
void PlayListModel::updateLayout() {
    emit layoutChanged();
}
