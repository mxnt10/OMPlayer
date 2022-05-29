#ifndef OMPLAYER_ABOUT_H
#define OMPLAYER_ABOUT_H

#include <QDialog>

#define PRG_NAME "Open Multimedia Player"
#define PRG_DESC "OMPlayer - Open Multimedia Player"
#define MAINTAINER "Mauricio Ferrari"
#define EMAIL "m10ferrari1200@gmail.com"
#define LICENSE "GNU General Public License Version 3 (GLPv3)"

class Button;


/**
 * Classe para o sobre.
 **********************************************************************************************************************/


class About : public QDialog {
Q_OBJECT
public:
    explicit About(QWidget *parent = nullptr);
    ~About() override;

signals:
    void emitclose();

private slots:
    static QString getTextMaintainer();
    static QString getDescription();
    void onClose();
};

#endif //OMPLAYER_ABOUT_H
