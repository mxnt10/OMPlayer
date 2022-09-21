#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QDir>
#include <QLabel>
#include <QListView>
#include <QModelIndex>

#include <PlayListUtils>

#include "Button.h"

#define DefDIR QDir::homePath() + "/.config/OMPlayer/playlist.qds"

class PlayList : public QWidget {
Q_OBJECT
public:
    explicit PlayList(QWidget *parent = nullptr);
    ~PlayList() override;

    enum ST {Default = 0, First = 1, Second = 2, Update = 3};
    void load(ST load = Default, const QString &url = DefDIR);
    void save(const QString &url = DefDIR);
    QString getItems(int s);
    qint64 setDuration();
    int setListSize();
    int selectItems();
    void selectClean();
    void selectCurrent(int indx);
    void insert(const QString &url, int row, qint64 duration = 0, const QString &format = nullptr, ST status = Default);
    void changeIcons();

protected:
    bool event(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    enum M3UFormat {M3U8 = 0, DetectFormat = 1};
    void load_m3u(const QString& file, M3UFormat format);
    void insertItemAt(const PlayListItem &item, int row);
    void setItemAt(const PlayListItem &item, int row);

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
    void removeSelectedItems(PlayList::ST status = Default);

private Q_SLOTS:
    void clearItems();
    void onSelect(const QModelIndex &index);
    void onAboutToPlay(const QModelIndex &index);

private:
    Button *clearBtn{};
    Button *addBtn{}, *removeBtn{};
    QLabel *cleanlist{};
    PlayListDelegate *delegate{};
    PlayListModel *model{};
    QListView *listView{};
    QString sum{}, actsum{};
    QStringList hashlist{};
    QWidget *wpls{};
    int maxRows{-1};
    int startsize{0}, startlistsize{0}, startpos{0};
    bool isshow{false}, resize{false};
};

#endif // PLAYLIST_H
