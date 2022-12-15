#ifndef OMPLAYER_SETTINGS_H
#define OMPLAYER_SETTINGS_H

#include <QtAV>
#include <QtAVWidgets>
#include <QDialog>
#include <QGraphicsOpacityEffect>
#include <QRadioButton>
#include <QTabWidget>
#include <Button>

#include "Decoder.h"

class Settings : public QDialog {
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

protected:
    void showEvent(QShowEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    static QString changeIconsStyle();

Q_SIGNALS:
    void emitclose();
    void emitvalue(const QString &value);
    void changethemeicon();

private Q_SLOTS:
    void onClose();
    void rendererSelect(Render &value);
    void setIcon(const QString &index);

private:
    QGraphicsOpacityEffect *effect{};
    QPropertyAnimation *animation{};
    QTabWidget *tab{};
    QRadioButton *opengl{}, *qglwidget2{}, *x11renderer{}, *xvideo{}, *direct2d{}, *gdi{}, *qglwidget{}, *widget{};
    Button *closebtn{};
    Decoder *decoder{};
    QtAV::VideoRenderer *vo{};
    struct Render *vid_map{};
    bool onclose{false};
};

#endif //OMPLAYER_SETTINGS_H
