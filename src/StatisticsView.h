#ifndef OMPLAYER_STATISTICSVIEW_H
#define OMPLAYER_STATISTICSVIEW_H

#include <QDialog>
#include <QGraphicsOpacityEffect>
#include <QThread>
#include <QtAV/Statistics.h>
#include <Button>
#include <TreeView>

#include "StatisticsWorker.h"
#include "Worker.h"

class StatisticsView : public QDialog {
Q_OBJECT

public:
    explicit StatisticsView(QWidget *parent = nullptr);
    ~StatisticsView() override;
    void setStatistics(const QtAV::Statistics &s);
    void setCurrentTime(int current);
    void changeIcons();

public Q_SLOTS:
    void setRightDB(int value);
    void setLeftDB(int value);
    void resetValues();

protected:
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

Q_SIGNALS:
    void emitFormat(const QString &format);
    void emitclose();

private Q_SLOTS:
    void setMd5(const QString &md5);
    void setFormat(const QString &format);
    void setItemValues(const QStringList &values, const QStringList &valuesVideo, const QStringList &valuesAudio);
    void onClose();

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
    Button *ratio{}, *screen{};
    QGraphicsOpacityEffect *effect{};
    QPropertyAnimation *animation{};
    QTabWidget *tab{};
    TreeView *view1{}, *view2{}, *view3{}, *view4{};
    QList<QTreeWidgetItem*> baseItems{};
    QList<QTreeWidgetItem*> videoItems{};
    QList<QTreeWidgetItem*> audioItems{};
    QList<QTreeWidgetItem*> metadata{};
    QString ctime{}, fsize{}, url{};
    QtAV::Statistics statistics{};
    int timer{0};
    QString vuleft{}, vuright{};
    bool onclose{false};

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
