#ifndef OMPLAYER_ABOUT_H
#define OMPLAYER_ABOUT_H

#include <QDialog>
#include <Button>

#define PRG_NAME "Open Multimedia Player"
#define PRG_DESC "OMPlayer - Open Multimedia Player"
#define MAINTAINER "Mauricio Ferrari"
#define EMAIL "m10ferrari1200@gmail.com"
#define LICENSE "GNU General Public License Version 3 (GLPv3)"

class About : public QDialog {
Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr);
    ~About() override;
    void changeIcons();

protected:
    void showEvent(QShowEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

Q_SIGNALS:
    void emitclose();

private Q_SLOTS:
    void onClose();

private:
    static QString getTextMaintainer();
    static QString getDescription();
    Button *closebtn{}, *qtavbtn{};
    bool onclose{false};
};

#endif //OMPLAYER_ABOUT_H
