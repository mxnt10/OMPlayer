#ifndef OMPLAYER_SETTINGS_HPP
#define OMPLAYER_SETTINGS_HPP

#include <QtAV>
#include <QtAVWidgets>
#include <QRadioButton>
#include <QTabWidget>
#include <Button>
#include <Dialog>

#include "Decoder.hpp"

class Settings : public Dialog {
Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings() override;
    [[nodiscard]] QVariantHash videoDecoderOptions() const;
    QVector<QtAV::VideoDecoderId> decoderPriorityNames();

    struct Render {
        const char* name;
        QtAV::VideoRendererId id;
        QRadioButton *btn;
    };

private:
    static QString changeIconsStyle();


#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"

Q_SIGNALS:
    void onclose();
    void emitvalue(const QString &value);
    void changethemeicon();

#pragma clang diagnostic pop


private Q_SLOTS:
    void rendererSelect(Settings::Render &value);
    void setIcon(const QString &index);

private:
    QTabWidget *tab{};
    QRadioButton *opengl{}, *qglwidget2{}, *x11renderer{}, *xvideo{}, *direct2d{}, *gdi{}, *qglwidget{}, *widget{};
    Button *closebtn{};
    Decoder *decoder{};
    QtAV::VideoRenderer *vo{};
    struct Render *vid_map{};
};

#endif //OMPLAYER_SETTINGS_HPP
