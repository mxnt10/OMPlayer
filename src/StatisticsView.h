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

protected:
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

signals:
    void emitclose();

private Q_SLOTS:
    void onClose();

private:
    static QStringList getBaseInfoKeys();
    static QVariantList getBaseInfoValues(const Statistics &s);
    static QStringList getCommonInfoKeys();
    static QStringList getVideoInfoKeys();
    static QList<QVariant> getVideoInfoValues(const Statistics &s);
    static QStringList getAudioInfoKeys();
    static QList<QVariant> getAudioInfoValues(const Statistics &s);
    static void initBaseItems(QList<QTreeWidgetItem*>* items);
    static QTreeWidgetItem* createNodeWithItems(QTreeWidget* view, const QString& name, const QStringList& itemNames, QList<QTreeWidgetItem*>* items = nullptr);

private:
    QTreeWidget *view{};
    QList<QTreeWidgetItem*> baseItems{};
    QList<QTreeWidgetItem*> videoItems{};
    QList<QTreeWidgetItem*> audioItems{};
    Statistics statistics{};
    QTreeWidgetItem *itemv{}, *itema{};
    QTreeWidgetItem *FPS{};
    int timer{0};
};

#endif // STATISTICSVIEW_H
