#ifndef OMPLAYER_STATISTICSVIEW_H
#define OMPLAYER_STATISTICSVIEW_H

#include <QThread>
#include <QtAV/Statistics.h>
#include <Button>
#include <Dialog>
#include <Label>
#include <TreeView>

#include "StatisticsWorker.h"
#include "Worker.hpp"

class StatisticsView : public Dialog {
Q_OBJECT

public:
    explicit StatisticsView(QWidget *parent = nullptr);
    ~StatisticsView() override;
    void setStatistics(const QtAV::Statistics &s = QtAV::Statistics());
    void setCurrentTime(int current);
    void setFile(const QString &file);
    void changeIcons();

public Q_SLOTS:
    void setRightDB(int value);
    void setLeftDB(int value);
    void resetValues();

protected:
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

Q_SIGNALS:
    void emitFormat(const QString &format, int duration);
    void emitclose();

private Q_SLOTS:
    void setMd5(const QString &md5);
    void setItemValues(const QStringList &values, const QStringList &valuesVideo, const QStringList &valuesAudio,
                       const QString &format, int duration);

private:
    void visibility();
    void settaginfos();
    void setSize();
    static QStringList getBaseInfoKeys();
    static QStringList getVideoInfoKeys();
    static QStringList getAudioInfoKeys();
    static QStringList getMetaDataKeys();
    static QVariantList getMetaDataValues(const QtAV::Statistics &s);
    static void initItems(QList<QTreeWidgetItem*> *items, const QStringList &itemlist);

private:
    Button *closebtn{};
    Label *ratio{}, *screen{};
    TreeView *view1{}, *view2{}, *view3{}, *view4{};
    QList<QTreeWidgetItem*> baseItems{}, videoItems{}, audioItems{}, metadata{};
    QtAV::Statistics statistics{}, currentStatistics{};
    QTabWidget *tab{};
    QString vuleft{}, vuright{}, url{};
    int timer{0};

    QList<int> fuhdw{7680, 8192, 10080}; //8k with
    QList<int> fuhdh{5120, 4320};        //8k heith
    QList<int> uhdpw{5120, 4800};        //5k with
    QList<int> uhdph{4096, 3840, 3200, 2700, 2560, 2160, 1440}; //5k heith
    QList<int> uhdw{3840, 4096, 3996, 3656}; // 4k with
    QList<int> uhdh{2160, 2664, 1714};       // 4k heith

    /** Suporte multithread */
    QThread *thread, *thread2;
    Worker *worker;
    StatisticsWorker *statisticsworker;
};

#endif //OMPLAYER_STATISTICSVIEW_H
