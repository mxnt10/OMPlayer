#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include "PlayListItem.h"


/**
 * Classe para o modelo da playlist.
 ***********************************************************************************************************************/


class PlayListModel : public QAbstractListModel {
Q_OBJECT
    Q_DISABLE_COPY(PlayListModel)

public:
    explicit PlayListModel(QObject *parent = nullptr);
    [[nodiscard]] QList<PlayListItem> items() const;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
    void updateLayout();

private:
    QList<PlayListItem> mediaItems;
};

#endif // PLAYLISTMODEL_H
