#ifndef OMPLAYER_SETTINGS_H
#define OMPLAYER_SETTINGS_H

#include <QtAV>
#include <QtAVWidgets>
#include <QDialog>

#include "Button.h"

class Settings : public QDialog {
Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings() override;

    struct Render {
        const char* name;
        QtAV::VideoRendererId id;
        Button *btn;
    };

signals:
    void emitclose();
    void emitvalue(const QString &value);

private Q_SLOTS:
    void onClose();
    void rendererSelect(Button *btn, const QString &name);
    void setOpenglWidget();
    void setQGLWidget2();
    void setDirect2D();
    void setGDI();
    void setXVideo();
    void setX11renderer();
    void setQGLWidget();
    void setWidget();

private:
    Button *opengl{}, *qglwidget2{};
    Button *x11renderer{}, *xvideo{};
    Button *direct2d{}, *gdi{};
    Button *qglwidget{}, *widget{};
    Button *closebtn{};
    QtAV::VideoRenderer *vo{};
    struct Render *vid_map{};
};

#endif //OMPLAYER_SETTINGS_H
