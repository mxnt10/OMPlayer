#ifndef STATISTICSVIEW_H
#define STATISTICSVIEW_H

#include <QDialog>
#include <QTreeWidget>
#include <QtAV/Statistics.h>

using namespace QtAV;

class StatisticsView : public QDialog {
Q_OBJECT
public:
    explicit StatisticsView(QWidget *parent = nullptr);
    void setStatistics(const Statistics &s);
    void setCurrentTime(int current);

protected:
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

signals:
    void emitclose();

private Q_SLOTS:
    void onClose();

private:
    void visibility();
    static QStringList getBaseInfoKeys();
    static QStringList getVideoInfoKeys();
    static QStringList getAudioInfoKeys();
    QVariantList getBaseInfoValues(const Statistics &s);
    static QVariantList getVideoInfoValues(const Statistics &s);
    static QVariantList getAudioInfoValues(const Statistics &s);
    static void initItems(QList<QTreeWidgetItem *> *items, const QStringList &itemlist);

private:
    QTabWidget *tab{};
    QTreeWidget *view1{}, *view2{}, *view3{};
    QList<QTreeWidgetItem*> baseItems{};
    QList<QTreeWidgetItem*> videoItems{};
    QList<QTreeWidgetItem*> audioItems{};
    QTreeWidgetItem *FPS{}, *CTIME{};
    QString ctime{}, fsize{}, url{};
    Statistics statistics{};
    int timer{0};
};

#endif // STATISTICSVIEW_H
