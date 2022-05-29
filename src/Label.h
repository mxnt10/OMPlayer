#ifndef OMPLAYER_LABEL_H
#define OMPLAYER_LABEL_H

#include <QLabel>

using namespace std;


/**
 * Classe Label.
 **********************************************************************************************************************/


class Label : public QLabel {
Q_OBJECT
public:
    explicit Label(const QFlag& align, int w = 0, const QString& tooltip = nullptr, const QString& text = nullptr);
    ~Label() override;

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

signals:
    void emitEnter();
    void emitLeave();

private:
    QString txt;
};

#endif //OMPLAYER_LABEL_H
