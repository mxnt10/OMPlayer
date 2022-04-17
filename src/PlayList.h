#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>
#include <QModelIndex>
#include <MediaInfoDLL.h>

#include "PlayListItem.h"

using namespace MediaInfoDLL;


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

namespace MediaInfoDLL {
    class MediaInfo;
}


/**
 * Classe da playlist.
 **********************************************************************************************************************/


class PlayList : public QWidget {
Q_OBJECT
public:
    explicit PlayList(QWidget *parent = nullptr);

    ~PlayList() override;

    QString getItems(int s);

    int setListSize();

    int selectItems();

    void selectClean();

    void selectPlay();

    void selectNext();

    void selectPrevious();

    void setIndex();

    void setSaveFile(const QString &file);

    void load();

    void save();

    void insert(const QString &url, int row = 0);

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

    void emitnohide();

public slots:
    void addItems(const QStringList &parms = QStringList());

private slots:
    void removeSelectedItems();

    void clearItems();

    void onSelect(const QModelIndex &index);

    void onAboutToPlay(const QModelIndex &index);

    void noHide();

private:
    Button *clearBtn, *removeBtn, *addBtn;
    ListView *listView;
    MediaInfo MI;
    PlayListModel *model;
    QModelIndex actualitem;
    QString duration, format, mfile;
    QWidget *wpls;
    int maxRows;
    bool isshow;
};

#endif // PLAYLIST_H
