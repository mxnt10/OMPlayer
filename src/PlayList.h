#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QListView>
#include <QModelIndex>
#include <PlayListUtils>

#include "Button.h"

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
    void changeIcons();

protected:
    bool event(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void insertItemAt(const PlayListItem &item, int row = 0);
    void setItemAt(const PlayListItem &item, int row = 0);

signals:
    void aboutToPlay(const QString &url);
    void firstPlay(const QString &url, int row);
    void selected(int item);
    void emitremove(int item);
    void emitstop();
    void emithide();
    void emithiden();
    void emitnohide();
    void enterListView();
    void leaveListView();
    void emitItems();

public Q_SLOTS:
    void addItems(const QStringList &parms = QStringList());
    void removeSelectedItems(bool update = false);

private Q_SLOTS:
    void clearItems();
    void onSelect(const QModelIndex &index);
    void onAboutToPlay(const QModelIndex &index);
    void enterList();
    void leaveList();

private:
    Button *clearBtn{};
    Button *addBtn{}, *removeBtn{};
    PlayListDelegate *delegate{};
    PlayListModel *model{};
    QListView *listView{};
    QString mfile{};
    QString sum{}, actsum{};
    QStringList hashlist{};
    QWidget *wpls{};
    int maxRows{-1};
    bool isshow{false};
};

#endif // PLAYLIST_H
