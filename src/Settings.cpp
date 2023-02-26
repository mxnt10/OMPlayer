#include <QComboBox>
#include <QGroupBox>
#include <QLayout>
#include <Label>
#include <Utils>

#include "Settings.hpp"

/**********************************************************************************************************************/


/** Classe principal das configurações */
Settings::Settings(QWidget *parent) : Dialog(parent) {
    setFixedWidth(400);
    connect(this, &Dialog::emitclose, [this](){ Q_EMIT onclose(); });
    decoder = new Decoder(this);
    renderer = new Rend::Renderer(this);
    subtitle = new Sett::Subtitle(this);
    connect(renderer, &Rend::Renderer::rendervalue, [this](const QString &s){ Q_EMIT emitvalue(s); });
    connect(subtitle, &Sett::Subtitle::changeColor, [this](const QColor &c){ Q_EMIT emitcolor(c); });
    connect(subtitle, &Sett::Subtitle::changeFont, [this](const QFont &f){ Q_EMIT emitfont(f); });
    connect(subtitle, &Sett::Subtitle::enableSub, [this](bool v){ Q_EMIT emitEnableSub(v); });
    connect(subtitle, &Sett::Subtitle::changeEngine, [this](const QString &e){ Q_EMIT emitEngine(e); });
    connect(subtitle, &Sett::Subtitle::changeCharset, [this](){ Q_EMIT emitCharset(); });


    /** Configurações de tema */
    auto *combotheme = new QComboBox();
    combotheme->addItems(Utils::subdirIcons());
    combotheme->setCurrentText(JsonTools::stringJson("theme"));
    combotheme->setStyleSheet(Utils::setStyle("global"));
    connect(combotheme, &QComboBox::currentTextChanged, this, &Settings::setIcon);


    /** Layout para a configuração do tema */
    auto *labeltheme = new Label(RIGHT, null, tr("Icon Themes") + ": ");
    auto *themes = new QGridLayout();
    themes->setContentsMargins(0, 50, 0, 0);
    themes->addWidget(labeltheme, 0, 0);
    themes->addWidget(combotheme, 0, 1);
    themes->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding), 1, 0);


    /** Definição dos grupos */
    auto *themebox = new QGroupBox(tr("Themes"));
    themebox->setLayout(themes);


    /** Itens gerais */
    auto *general = new QWidget();
    auto *opgeneral = new QVBoxLayout(general);
    opgeneral->addWidget(themebox);


   /** Organização por abas */
    tab = new QTabWidget();
    tab->addTab(general, tr("General"));
    tab->addTab(decoder, tr("Decoder"));
    tab->addTab(renderer, tr("Renderer"));
    tab->addTab(subtitle, tr("Subtitle"));
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


/** Modificando as configurações de temas */
void Settings::setIcon(const QString &index) {
    qDebug("%s(%sSettings%s)%s::%sTema de ícones %s ...\033[0m", GRE, RED, GRE, RED, ORA, STR(index));
    JsonTools::stringJson("theme", index);

    Utils::initUtils(Utils::UpdateTheme);
    Utils::changeIcon(closebtn, "apply");
    decoder->changeIcons();
    renderer->changeIcons();
    subtitle->changeIcons();

    Q_EMIT changethemeicon();
}


/** Retorna as opções de decodificação e a prioridade de execução */
QVariantHash Settings::videoDecoderOptions() const { return decoder->videoDecoderOptions(); }
QVector<QtAV::VideoDecoderId> Settings::decoderPriorityNames() { return decoder->decoderPriorityNames(); }
