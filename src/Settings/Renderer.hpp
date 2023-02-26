#ifndef OMPLAYER_RENDERER_HPP
#define OMPLAYER_RENDERER_HPP

#include <QtAV>
#include <QRadioButton>
#include <QWidget>

namespace Rend {
    class Renderer : public QWidget {
    Q_OBJECT

    public:
        explicit Renderer(QWidget *parent = nullptr);
        ~Renderer() override;
        void changeIcons();

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
        void rendervalue(const QString &value);

    #pragma clang diagnostic pop


    private Q_SLOTS:
        void rendererSelect(Renderer::Render &value);

    private:
        QtAV::VideoRenderer *vo{};
        struct Render *vid_map{};
        QRadioButton *opengl{},
                     *qglwidget2{},
                     *x11renderer{},
                     *xvideo{},
                     *direct2d{},
                     *gdi{},
                     *qglwidget{},
                     *widget{};
    };
}

#endif //OMPLAYER_RENDERER_HPP
