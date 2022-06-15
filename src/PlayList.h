#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QListView>
#include <QModelIndex>

#include "Button.h"
#include "PlayListItem.h"
#include "PlayListModel.h"


class PlayList : public QWidget {
Q_OBJECT
public:
    explicit PlayList(QWidget *parent = nullptr);
    ~PlayList() override;
    QString getItems(int s);
    qint64 setDuration();
    int setListSize();
    int selectItems();
    void load(bool second = false);
    void save();
    void selectClean();
    void selectCurrent(int indx);
    void insert(const QString &url, int row = 0, qint64 duration = 0, const QString &format = nullptr);
    void insertItemAt(const PlayListItem &item, int row = 0);
    void setItemAt(const PlayListItem &item, int row = 0);

protected:
    bool event(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void aboutToPlay(const QString &url);
    void firstPlay(const QString &url, int row);
    void selected(int item);
    void emitremove(int item);
    void emitstop();
    void emithide();
    void emithiden();
    void emitnohide();

public slots:
    void addItems(const QStringList &parms = QStringList());
    void removeSelectedItems(bool update = false);

private slots:
    void clearItems();
    void onSelect(const QModelIndex &index);
    void onAboutToPlay(const QModelIndex &index);
    void noHide();

private:
    Button *clearBtn{};
    Button *removeBtn{};
    Button *addBtn{};
    PlayListModel *model{};
    QListView *listView{};
    QString mfile{};
    QString sum{};
    QString actsum{};
    QWidget *wpls{};
    int maxRows{-1};
    bool isshow{false};
};

#endif // PLAYLIST_H
