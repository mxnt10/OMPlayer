#ifndef OMPLAYER_STATISTICSVIEW_HPP
#define OMPLAYER_STATISTICSVIEW_HPP

#include <QtAV>
#include <QThread>
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
    void setFile(const QString &file);
    void changeIcons();
    void resetValues();

public Q_SLOTS:
    void setRightDB(float value);
    void setLeftDB(float value);

protected:
    void hideEvent(QHideEvent *event) override;


#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"

Q_SIGNALS:
    void emitFormat(const QString &format, int duration);
    void onclose();

#pragma clang diagnostic pop


private Q_SLOTS:
    void showDialog();
    void onStatistics();
    void setMd5(const QString &md5);
    void setItemValues(const QStringList &values, const QStringList &valuesVideo, const QStringList &valuesAudio,
                       const QStringList &valuesDual, const QStringList &metadataval, const QString &format);

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
    QtAV::AVPlayer *st{};
    QtAV::Statistics statistics{};
    Button *closebtn{};
    Label *ratio{}, *screen{};
    TreeView *view1{}, *view2{}, *view3{}, *view4{}, *view5{};
    QList<QTreeWidgetItem*> baseItems{}, videoItems{}, audioItems{}, audioDual{}, metadata{};
    QList<QList<QTreeWidgetItem *>> itemTab{};
    QList<TreeView *> itemView{};
    QTabWidget *tab{};
    QString vuleft{}, vuright{}, url{};

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

#endif //OMPLAYER_STATISTICSVIEW_HPP
