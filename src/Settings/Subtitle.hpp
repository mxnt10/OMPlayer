#ifndef OMPLAYER_SUBTITLE_HPP
#define OMPLAYER_SUBTITLE_HPP

#include <QCheckBox>
#include <QWidget>

namespace Sett {
    class Subtitle : public QWidget {
    Q_OBJECT

    public:
        explicit Subtitle(QWidget *parent = nullptr);
        ~Subtitle() override;
        void changeIcons();


#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"

    Q_SIGNALS:
        void enableSub(bool val);
        void changeEngine(const QString &engine);
        void changeCharset();
        void changeFont(QFont font);
        void changeColor(QColor color);

#pragma clang diagnostic pop


    private Q_SLOTS:
        void setColor();
        void setFont();
        void setEnable(bool val);
        void setEngine(const QString &engine);
        void setCharset(const QString &charset);
        static void setAutoLoad(bool val);

    private:
        static QString changeIconsStyle();
        QCheckBox *checkEnable{}, *checkAutoLoad{};
    };
}

#endif //OMPLAYER_SUBTITLE_HPP
