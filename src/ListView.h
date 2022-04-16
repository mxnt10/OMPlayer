#ifndef OMPLAYER_LISTVIEW_H
#define OMPLAYER_LISTVIEW_H

#include <QListView>


/**
 * Classe que vai servir como assistente para emissão de sinais.
 **********************************************************************************************************************/


class ListView : public QListView {
Q_OBJECT
public:
    ListView();

    ~ListView() override;

protected:
    void enterEvent(QEvent *event) override;

    void leaveEvent(QEvent *event) override;

signals:

    void emitEnter();

    void emitLeave();
};

#endif //OMPLAYER_LISTVIEW_H
