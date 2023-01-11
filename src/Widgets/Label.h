#ifndef OMPLAYER_LABEL_H
#define OMPLAYER_LABEL_H

#include <QLabel>

class Label : public QLabel {
Q_OBJECT

public:
    explicit Label(const QFlag& align, int w = 0, const QString& text = nullptr);
    explicit Label(const QFlag& align, int w = 0, int h = 0, QWidget *parent = nullptr);
    explicit Label(const QFlag& align, const QString &local);
    explicit Label(QWidget *parent = nullptr);
    ~Label() override;
};

class EmptyList : public QLabel {
Q_OBJECT

public:
    explicit EmptyList(const QString& text, QWidget *parent = nullptr);
    ~EmptyList() override;
};

#endif //OMPLAYER_LABEL_H
