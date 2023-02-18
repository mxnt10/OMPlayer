#ifndef OMPLAYER_ABOUT_HPP
#define OMPLAYER_ABOUT_HPP

#include <Button>
#include <Dialog>

#define PRG_NAME "Open Multimedia Player"
#define PRG_DESC "OMPlayer - Open Multimedia Player"
#define MAINTAINER "Mauricio Ferrari"
#define EMAIL "m10ferrari1200@gmail.com"
#define LICENSE "GNU General Public License Version 3 (GLPv3)"

class About : public Dialog {
Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr);
    ~About() override;
    void changeIcons();


#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"

Q_SIGNALS:
    void onclose();

#pragma clang diagnostic pop


private:
    static QString getTextMaintainer();
    static QString getDescription();
    Button *closebtn{}, *qtavbtn{};
};

#endif //OMPLAYER_ABOUT_HPP
