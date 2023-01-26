#ifndef OMPLAYER_CWIDGET_HPP
#define OMPLAYER_CWIDGET_HPP

#include <QWidget>

class CWidget : public QWidget {
Q_OBJECT
public:
    explicit CWidget(QWidget *parent = nullptr, QFlags<Qt::WindowType> type = QFlags<Qt::WindowType>());
    explicit CWidget(QFlags<Qt::WindowType> type);
};

class TrackWidget : public QWidget {
Q_OBJECT
public:
    explicit TrackWidget(QWidget *parent = nullptr);
};

#endif // OMPLAYER_CWIDGET_HPP
