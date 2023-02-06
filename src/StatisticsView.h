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
    void setCurrentStatistics(const QtAV::Statistics &s) { currentStatistics = s; };
    void setFile(const QString &file);
    void changeIcons();
    void resetValues();

public Q_SLOTS:
    void setRightDB(float value);
    void setLeftDB(float value);

protected:
    void hideEvent(QHideEvent *event) override;

Q_SIGNALS:
    void emitFormat(const QString &format, int duration);
    void emitclose();

private Q_SLOTS:
    void setMd5(const QString &md5);
    void setItemValues(const QStringList &values, const QStringList &valuesVideo, const QStringList &valuesAudio,
                       const QStringList &valuesDual, const QStringList &metadataval,
                       const QString &format, int duration, const QStringList &rat);

private:
    enum TypeSize{ NormalSize = 0, InitialSize = 1, HeaderSize = 2 };
    void visibility();
    void settaginfos();
    void setSize(TypeSize size);
    static QStringList getBaseInfoKeys();
    static QStringList getVideoInfoKeys();
    static QStringList getAudioInfoKeys();
    static QStringList getMetaDataKeys();
    static void initItems(QList<QTreeWidgetItem*> *items, const QStringList &itemlist);

private:
    Button *closebtn{};
    Label *ratio{}, *screen{};
    TreeView *view1{}, *view2{}, *view3{}, *view4{}, *view5{};
    QList<QTreeWidgetItem*> baseItems{}, videoItems{}, audioItems{}, audioDual{}, metadata{};
    QtAV::Statistics statistics{}, currentStatistics{};
    QTabWidget *tab{};
    QString vuleft{}, vuright{}, url{};
    QStringList saveratio{};

    QList<int> fuhdw{7680, 8192, 10080}; //8k with
    QList<int> fuhdh{5120, 4320};        //8k heith
    QList<int> uhdpw{5120, 4800};        //5k with
    QList<int> uhdph{4096, 3840, 3200, 2700, 2560, 2160, 1440}; //5k heith
    QList<int> uhdw{3840, 4096, 3996, 3656}; // 4k with
    QList<int> uhdh{2160, 2664, 1714};       // 4k heith

    QThread *thread, *thread2;
    Worker *worker;
    StatisticsWorker *statisticsworker;
};

#endif //OMPLAYER_STATISTICSVIEW_H
