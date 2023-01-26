#ifndef OMPLAYER_LABEL_HPP
#define OMPLAYER_LABEL_HPP

#include <QLabel>

#define null (-1)

class Label : public QLabel {
Q_OBJECT

public:
    explicit Label(const QFlag& align, int w = null, const QString& text = nullptr);
    explicit Label(const QFlag& align, int w = null, int h = null, QWidget *parent = nullptr);
    explicit Label(const QFlag& align, const QString &local);
    explicit Label(QWidget *parent = nullptr);
    ~Label() override;
};

class EmptyList : public QLabel {
Q_OBJECT

public:
    explicit EmptyList(const QString& text, QWidget *parent = nullptr);
};

#endif //OMPLAYER_LABEL_HPP
