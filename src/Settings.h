#ifndef OMPLAYER_SETTINGS_H
#define OMPLAYER_SETTINGS_H

#include <QDialog>
#include "Button.h"


class Settings : public QDialog {
Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings() override;

signals:
    void emitclose();
    void emitvalue(const QString &value);

private slots:
    void onClose();
    void rendererSelect(Button *btn, const QString &name, const QString &desc);
    void setOpenglWidget();
    void setQGLWidget2();
    void setDirect2D();
    void setGDI();
    void setXVideo();
    void setX11renderer();
    void setQGLWidget();
    void setWidget();

private:
    Button *openglwidget{};
    Button *qglwidget2{};
    Button *x11renderer{};
    Button *xvideo{};
    Button *direct2d{};
    Button *gdi{};
    Button *qglwidget{};
    Button *widget{};
};

#endif //OMPLAYER_SETTINGS_H
