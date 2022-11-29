#ifndef STATISTICSVIEW_H
#define STATISTICSVIEW_H

#include <QDialog>
#include <QThread>
#include <QtAV/Statistics.h>
#include <Button>
#include <TreeView>

#include "Worker.h"

class StatisticsView : public QDialog {
Q_OBJECT

public:
    explicit StatisticsView(QWidget *parent = nullptr);
    ~StatisticsView() override;
    void setStatistics(const QtAV::Statistics &s);
    void setCurrentTime(int current);
    void changeIcons();

protected:
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

Q_SIGNALS:
    void emitFormat(const QString &format);
    void emitclose();

private Q_SLOTS:
    void setMd5(const QString &md5);
    void setFormat(const QString &format);
    void onClose();

private:
    void visibility();
    void settaginfos();
    static QStringList getBaseInfoKeys();
    static QStringList getVideoInfoKeys();
    static QStringList getAudioInfoKeys();
    QVariantList getBaseInfoValues(const QtAV::Statistics &s);
    static QVariantList getVideoInfoValues(const QtAV::Statistics &s);
    static QVariantList getAudioInfoValues(const QtAV::Statistics &s);
    static void initItems(QList<QTreeWidgetItem*> *items, const QStringList &itemlist);

private:
    Button *closebtn{};
    Button *ratio{}, *screen{};
    QTabWidget *tab{};
    TreeView *view1{}, *view2{}, *view3{};
    QList<QTreeWidgetItem*> baseItems{};
    QList<QTreeWidgetItem*> videoItems{};
    QList<QTreeWidgetItem*> audioItems{};
    QTreeWidgetItem *FPS{}, *CTIME{}, *MD5{}, *FORMAT{};
    QString ctime{}, fsize{}, url{};
    QtAV::Statistics statistics{};
    int timer{0};

    QList<int> fuhdw{7680, 8192, 10080}; //8k with
    QList<int> fuhdh{5120, 4320};        //8k heith
    QList<int> uhdpw{5120, 4800};        //5k with
    QList<int> uhdph{4096, 3840, 3200, 2700, 2560, 2160, 1440}; //5k heith
    QList<int> uhdw{3840, 4096, 3996, 3656}; // 4k with
    QList<int> uhdh{2160, 2664, 1714};       // 4k heith

    /** Suporte multithread */
    QThread *thread;
    Worker *worker;
};

#endif // STATISTICSVIEW_H
