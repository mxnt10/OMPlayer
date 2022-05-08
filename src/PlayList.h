#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>
#include <QModelIndex>
#include "PlayListItem.h"


/**
 * Declaração das classes.
 **********************************************************************************************************************/


QT_BEGIN_NAMESPACE
class QModelIndex;
class QString;
class QWidget;
QT_END_NAMESPACE

class Button;
class ListView;
class PlayListModel;


/**
 * Classe da playlist.
 **********************************************************************************************************************/


class PlayList : public QWidget {
Q_OBJECT
public:
    explicit PlayList(QWidget *parent = nullptr);
    ~PlayList() override;
    QString getItems(int s);
    qint64 setDuration();
    int setListSize();
    int selectItems();
    void selectClean();
    void selectPlay();
    void selectNext();
    void selectPrevious();
    void setIndex();
    void load(bool second = false);
    void save();
    void selectCurrent(int indx);
    void setSaveFile(const QString &file);
    void insert(const QString &url, int row = 0, qint64 duration = 0, const QString &format = nullptr);
    void insertItemAt(const PlayListItem &item, int row = 0);
    void setItemAt(const PlayListItem &item, int row = 0);

protected:
    bool event(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void aboutToPlay(const QString &url);
    void firstPlay(const QString &url);
    void selected(int item);
    void emithide();
    void emithiden();
    void emitnohide();

public slots:
    void addItems(const QStringList &parms = QStringList());
    void removeSelectedItems();

private slots:
    void clearItems();
    void onSelect(const QModelIndex &index);
    void onAboutToPlay(const QModelIndex &index);
    void noHide();

private:
    Button *clearBtn, *removeBtn, *addBtn;
    ListView *listView;
    PlayListModel *model;
    QModelIndex actualitem;
    QString mfile, sum, actsum;
    QWidget *wpls;
    int maxRows;
    bool isshow;
};

#endif // PLAYLIST_H
