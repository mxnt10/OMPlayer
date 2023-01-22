#ifndef OMPLAYER_PLAYLISTMODEL_H
#define OMPLAYER_PLAYLISTMODEL_H

#include <QAbstractListModel>

#include "PlayListItem.h"

class PlayListModel : public QAbstractListModel {
Q_OBJECT
Q_DISABLE_COPY(PlayListModel)

public:
    explicit PlayListModel(QObject *parent = nullptr);
    ~PlayListModel() override;
    [[nodiscard]] QList<PlayListItem> items() const { return mediaItems; };
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
    void updateLayout();

private:
    QList<PlayListItem> mediaItems;
};

#endif // OMPLAYER_PLAYLISTMODEL_H
