#ifndef OMPLAYER_CWIDGET_H
#define OMPLAYER_CWIDGET_H

#include <QWidget>

class CWidget : public QWidget {
Q_OBJECT
public:
    explicit CWidget(QWidget *parent = nullptr, QFlags<Qt::WindowType> type = QFlags<Qt::WindowType>());
    explicit CWidget(QFlags<Qt::WindowType> type);
    ~CWidget() override;
};

#endif // OMPLAYER_CWIDGET_H
