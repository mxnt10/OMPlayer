#ifndef OMPLAYER_WIDGET_H
#define OMPLAYER_WIDGET_H

#include <QWidget>


/**
 * Classe que vai servir como assistente para emissão de sinais.
 **********************************************************************************************************************/


class Widget : public QWidget {
Q_OBJECT

protected:
    void enterEvent(QEvent *event) override;

    void leaveEvent(QEvent *event) override;

signals:

    void emitEnter();

    void emitLeave();
};

#endif //OMPLAYER_WIDGET_H
