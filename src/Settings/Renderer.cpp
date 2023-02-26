#include <QtAVWidgets>
#include <QGroupBox>
#include <QLayout>
#include <QSpacerItem>
#include <Utils>

#include "Renderer.hpp"

/**********************************************************************************************************************/


/** Construtor */
Rend::Renderer::Renderer(QWidget *parent) : QWidget(parent) {

    /** Estrutura das renderizações */
    vid_map = new Render[8];
    vid_map[0] = {"OpenGL",     QtAV::VideoRendererId_OpenGLWidget, opengl      };
    vid_map[1] = {"QGLWidget2", QtAV::VideoRendererId_GLWidget2,    qglwidget2  };
    vid_map[2] = {"Direct2D",   QtAV::VideoRendererId_Direct2D,     direct2d    };
    vid_map[3] = {"GDI",        QtAV::VideoRendererId_GDI,          gdi         };
    vid_map[4] = {"XVideo",     QtAV::VideoRendererId_XV,           xvideo      };
    vid_map[5] = {"X11",        QtAV::VideoRendererId_X11,          x11renderer };
    vid_map[6] = {"QGLWidget",  QtAV::VideoRendererId_GLWidget,     qglwidget   };
    vid_map[7] = {"Widget",     QtAV::VideoRendererId_Widget,       widget      };


    /** Layout dos renderizadores */
    auto *oprenderer = new QGridLayout();
    oprenderer->setContentsMargins(0, 20, 0, 0);
    auto *rendererbox = new QGroupBox(tr("Options"));
    rendererbox->setLayout(oprenderer);
    auto *rend = new QVBoxLayout();
    rend->addWidget(rendererbox);


    /** Selecionar renderizações existentes */
    for (int i = 0; i < 8; ++i) {
        vo = QtAV::VideoRenderer::create(vid_map[i].id);
        if (vo && vo->isAvailable()) {
            vid_map[i].btn = new QRadioButton(vid_map[i].name);
            vid_map[i].btn->setFocusPolicy(Qt::NoFocus);
            vid_map[i].btn->setStyleSheet(changeIconsStyle());

            /** Tenque usar essa array com os números explícitos senão dá erro */
            int nm[8] = {0, 1, 2, 3, 4, 5, 6, 7};
            connect(vid_map[nm[i]].btn, &QRadioButton::toggled, [this, nm, i](){ rendererSelect(vid_map[nm[i]]); });

            if (QString::compare(JsonTools::stringJson("renderer"), vid_map[i].name) == 0)
                vid_map[i].btn->setChecked(true);
            oprenderer->addWidget(vid_map[i].btn, i, 0, LEFT);
        }
    }
    oprenderer->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding), 8, 0);
    this->setLayout(rend);
}


/** Destrutor */
Rend::Renderer::~Renderer() = default;


/**********************************************************************************************************************/


/** Selecionando o renderizador */
void Rend::Renderer::rendererSelect(Renderer::Render &value) {
    if (QString::compare(JsonTools::stringJson("renderer"), value.name) == 0) return;
    qDebug("%s(%sRenderer%s)%s::%sSelecionando renderizador %s ...\033[0m", GRE, RED, GRE, RED, ORA, STR(value.name));

    JsonTools::stringJson("renderer", value.name);
    Q_EMIT rendervalue(value.name);
}


/** Mudando o ícone nos estilos */
QString Rend::Renderer::changeIconsStyle() {
    QString str{Utils::setStyle("radio")};
    QString theme{Utils::setThemeName()};

    QString ru{Utils::setIconTheme(theme, "radio-unselect")};
    if(!ru.isEmpty()) str.replace(QRegExp("/\\* _RADIO_UNCHECKED__ \\*/"), "image: url(" + ru + ");");
    else return{};

    QString rs{Utils::setIconTheme(theme, "radio-select")};
    if(!rs.isEmpty()) str.replace(QRegExp("/\\* _RADIO_CHECKED_ \\*/"), "image: url(" + rs + ");");
    else return{};

    return str;
}


/** Alterando os ícones */
void Rend::Renderer::changeIcons() {
    qDebug("%s(%sRenderer%s)%s::%sAlterando ícones em Renderer ...\033[0m", GRE, RED, GRE, RED, ORA);
    for (int i = 0; i < 8; ++i) {
        vo = QtAV::VideoRenderer::create(vid_map[i].id);
        if (vo && vo->isAvailable()) vid_map[i].btn->setStyleSheet(changeIconsStyle());
    }
}
