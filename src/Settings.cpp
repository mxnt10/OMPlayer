#include <QComboBox>
#include <QGroupBox>
#include <QLayout>
#include <Label>
#include <Utils>

#include "Settings.hpp"

using namespace QtAV;


/**********************************************************************************************************************/


/** Classe principal das configurações */
Settings::Settings(QWidget *parent) : Dialog(parent) {
    setFixedWidth(400);
    connect(this, &Dialog::emitclose, [this](){ Q_EMIT onclose(); });
    decoder = new Decoder(this);


    /** Estrutura das renderizações */
    vid_map = new Render[8];
    vid_map[0] = {"OpenGL",     VideoRendererId_OpenGLWidget, opengl      };
    vid_map[1] = {"QGLWidget2", VideoRendererId_GLWidget2,    qglwidget2  };
    vid_map[2] = {"Direct2D",   VideoRendererId_Direct2D,     direct2d    };
    vid_map[3] = {"GDI",        VideoRendererId_GDI,          gdi         };
    vid_map[4] = {"XVideo",     VideoRendererId_XV,           xvideo      };
    vid_map[5] = {"X11",        VideoRendererId_X11,          x11renderer };
    vid_map[6] = {"QGLWidget",  VideoRendererId_GLWidget,     qglwidget   };
    vid_map[7] = {"Widget",     VideoRendererId_Widget,       widget      };


    /** Layout da renderização */
    auto *performance = new QWidget();
    auto *oprenderer = new QGridLayout();
    oprenderer->setContentsMargins(0, 20, 0, 0);


    /** Selecionar renderizações existentes */
    for (int i = 0; i < 8; ++i) {
        vo = VideoRenderer::create(vid_map[i].id);
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


    /** Configurações de tema */
    auto *combotheme = new QComboBox();
    combotheme->addItems(Utils::subdirIcons());
    combotheme->setCurrentText(JsonTools::stringJson("theme"));
    combotheme->setStyleSheet(Utils::setStyle("global"));
    connect(combotheme, &QComboBox::currentTextChanged, this, &Settings::setIcon);

    auto *labeltheme = new Label(RIGHT, null, tr("Icon Themes") + ": ");
    auto *themes = new QGridLayout();
    themes->setContentsMargins(0, 50, 0, 0);
    themes->addWidget(labeltheme, 0, 0);
    themes->addWidget(combotheme, 0, 1);
    themes->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding), 1, 0);


    /** Definição dos grupos */
    auto *themebox = new QGroupBox(tr("Themes"));
    themebox->setLayout(themes);
    auto *rendererbox = new QGroupBox(tr("Renderer"));
    rendererbox->setLayout(oprenderer);


    /** Opções de performance */
    auto *renderer = new QVBoxLayout(performance);
    renderer->addWidget(rendererbox);


    /** Itens gerais */
    auto *general = new QWidget();
    auto *opgeneral = new QVBoxLayout(general);
    opgeneral->addWidget(themebox);


   /** Organização por abas */
    tab = new QTabWidget();
    tab->addTab(general, tr("General"));
    tab->addTab(decoder, tr("Decoder"));
    tab->addTab(performance, tr("Performance"));
    tab->setStyleSheet(Utils::setStyle("tab"));


    /** Botão para fechar a janela */
    closebtn = new Button(Button::NormalBtn, 32, "apply");
    connect(closebtn, &Button::pressed, this, &Dialog::onClose);


    /** Layout para organização das configurações */
    auto *fore = new QGridLayout();
    fore->setMargin(10);
    fore->addWidget(tab, 0, 0);
    fore->addWidget(closebtn, 1, 0, RIGHT);


    /** Widget para o plano de fundo */
    auto *found = new QWidget();
    found->setStyleSheet(Utils::setStyle("widget"));


    /** Layout principal */
    auto *layout = new QGridLayout();
    layout->setMargin(0);
    layout->addWidget(found, 0, 0);
    layout->addLayout(fore, 0, 0);
    setLayout(layout);
}


/** Destrutor */
Settings::~Settings() = default;


/**********************************************************************************************************************/


/** Resetando os botões */
void Settings::rendererSelect(Render &value) {
    if (QString::compare(JsonTools::stringJson("renderer"), value.name) == 0) return;
    qDebug("%s(%sSettings%s)%s::%sSelecionando renderizador %s ...\033[0m", GRE, RED, GRE, RED, ORA, STR(value.name));

    JsonTools::stringJson("renderer", value.name);
    Q_EMIT emitvalue(value.name);
}


/** Modificando as configurações de temas */
void Settings::setIcon(const QString &index) {
    qDebug("%s(%sSettings%s)%s::%sTema de ícones %s ...\033[0m", GRE, RED, GRE, RED, ORA, STR(index));
    JsonTools::stringJson("theme", index);

    Utils::initUtils(Utils::UpdateTheme);
    Utils::changeIcon(closebtn, "apply");
    decoder->changeIcons();

    for (int i = 0; i < 8; ++i) {
        vo = VideoRenderer::create(vid_map[i].id);
        if (vo && vo->isAvailable()) vid_map[i].btn->setStyleSheet(changeIconsStyle());
    }

    Q_EMIT changethemeicon();
}


/** Retorna as opções de decodificação e a prioridade de execução */
QVariantHash Settings::videoDecoderOptions() const { return decoder->videoDecoderOptions(); }
QVector<QtAV::VideoDecoderId> Settings::decoderPriorityNames() { return decoder->decoderPriorityNames(); }


/** Mudando o ícone nos estilos */
QString Settings::changeIconsStyle() {
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
