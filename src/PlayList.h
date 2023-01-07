#ifndef OMPLAYER_PLAYLIST_H
#define OMPLAYER_PLAYLIST_H

#include <QDir>
#include <QFileDialog>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QModelIndex>
#include <PlayListUtils>
#include <Button>
#include <ListView>

#define DefDIR QDir::homePath() + "/.config/OMPlayer/playlist.qds"

class PlayList : public QWidget {
Q_OBJECT

public:
    explicit PlayList(QWidget *parent = nullptr);
    ~PlayList() override;

    enum STATUS {Default = 0, First = 1, Second = 2};
    void load(PlayList::STATUS load = PlayList::Default, const QString &url = DefDIR);
    void save(const QString &url = DefDIR);
    QString getItems(int s);
    int setListSize();
    int selectItems();
    void selectClean();
    void selectCurrent(int indx);
    void insert(const QString &url, int row, qint64 duration = 0);
    void updateItems(int row, qint64 duration, const QString &format);
    void changeIcons();
    void hideFade();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    enum M3UFormat {M3U8 = 0, DetectFormat = 1};
    enum PLS {Show = 0, Hide = 1};
    void load_m3u(const QString& file, M3UFormat format);
    void insertItemAt(const PlayListItem &item, int row);
    void fadePls(PLS option);

Q_SIGNALS:
    void aboutToPlay(const QString &url);
    void firstPlay(const QString &url, int row);
    void selected(int item);
    void emitremove(int item);
    void emitstop();
    void emithide();
    void emitnohide();
    void enterListView();
    void leaveListView();
    void nomousehide();

public Q_SLOTS:
    void getFiles();
    void addItems(const QStringList &files);
    void removeSelectedItems();

private Q_SLOTS:
    void clearItems();
    void onSelect(const QModelIndex &index);
    void onAboutToPlay(const QModelIndex &index);

private:
    Button *clearBtn{}, *addBtn{}, *removeBtn{};
    ListView *listView{};
    PlayListDelegate *delegate{};
    PlayListModel *model{};
    QFileDialog *diag{};
    QGraphicsOpacityEffect *effect{};
    QLabel *cleanlist{};
    QWidget *wpls{};
    int startsize{0}, startlistsize{0}, startpos{0}, rmRows{0};
    bool isshow{false}, resize{false};
};

#endif //OMPLAYER_PLAYLIST_H
