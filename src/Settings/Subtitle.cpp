#include <QColorDialog>
#include <QComboBox>
#include <QFontDialog>
#include <QGroupBox>
#include <QLayout>
#include <QTextCodec>
#include <Label>
#include <Utils>

#include "Subtitle.hpp"

/**********************************************************************************************************************/


/** Construtor */
Sett::Subtitle::Subtitle(QWidget *parent) : QWidget(parent) {

    /** Opções gerais */
    checkEnable = new QCheckBox(tr("Enable"));
    checkAutoLoad = new QCheckBox(tr("AutoLoad"));
    checkEnable->setChecked(JsonTools::boolJson("sub_enable"));
    checkEnable->setStyleSheet(changeIconsStyle());
    checkAutoLoad->setChecked(JsonTools::boolJson("sub_autoload"));
    checkAutoLoad->setStyleSheet(changeIconsStyle());
    connect(checkEnable, &QCheckBox::toggled, this, &Subtitle::setEnable);
    connect(checkAutoLoad, &QCheckBox::toggled, this, &Subtitle::setAutoLoad);


    /** Opções de engine */
    auto *labelengine = new Label(RIGHT, null, tr("Engine") + ": ");
    auto *comboengine = new QComboBox();
    comboengine->setStyleSheet(Utils::setStyle("global"));
    comboengine->addItems({"FFmpeg", "LibASS"});
    comboengine->setCurrentText(JsonTools::stringJson("sub_engine"));
    connect(comboengine, &QComboBox::currentTextChanged, this, &Subtitle::setEngine);


    /** Opções de conjuntos de caracteres */
    auto *labelcharset = new Label(RIGHT, null, tr("Charset") + ": ");
    auto *combocharset = new QComboBox();
    combocharset->setStyleSheet(Utils::setStyle("global"));
    combocharset->addItems({"Auto Detect", "System"});
    foreach(const QByteArray &it, QTextCodec::availableCodecs()) combocharset->addItem(QString::fromLocal8Bit(it));
    combocharset->setCurrentText(JsonTools::stringJson("sub_charset"));
    connect(combocharset, &QComboBox::currentTextChanged, this, &Subtitle::setCharset);


    /** Botão para selecionar a cor, esses não vão com ícone */
    auto colorbtn = new QPushButton();
    colorbtn->setText("Color");
    connect(colorbtn, &QPushButton::clicked, this, &Subtitle::setColor);


    /** Botão para selecionar a fonte */
    auto fontbtn = new QPushButton();
    fontbtn->setText("Font");
    connect(fontbtn, &QPushButton::clicked, this, &Subtitle::setFont);


    /** Layout das opções gerais */
    auto op = new QGridLayout();
    op->setSpacing(10);
    op->setContentsMargins(0, 20, 0, 20);
    op->addWidget(checkEnable, 0, 0);
    op->addWidget(checkAutoLoad, 1, 0);
    op->addWidget(labelengine, 2, 0);
    op->addWidget(comboengine, 2, 1);
    op->addWidget(labelcharset, 3, 0);
    op->addWidget(combocharset, 3, 1);


    /** Layout das custamizações */
    auto ff = new QGridLayout();
    ff->setContentsMargins(0, 20, 0, 0);
    ff->addWidget(fontbtn, 0, 0);
    ff->addWidget(colorbtn, 0, 1);


    /** Criando os grupos */
    auto *option = new QGroupBox(tr("Options"));
    option->setLayout(op);
    auto *custom = new QGroupBox(tr("Custom FFmpeg"));
    custom->setLayout(ff);


    /** Layout principal */
    auto *sub = new QVBoxLayout();
    sub->addWidget(option);
    sub->addWidget(custom);
    setLayout(sub);
}


/** Destrutor */
Sett::Subtitle::~Subtitle() = default;


/**********************************************************************************************************************/


/** Setando uma cor para a legenda */
void Sett::Subtitle::setColor() {
    QColorDialog cor;
    if (QString::compare(JsonTools::stringJson("sub_color"), "undefined") != 0) {
        QStringList l{JsonTools::stringJson("sub_color").split("-")};
        cor.setCurrentColor(QColor(l[0].toInt(), l[1].toInt(), l[2].toInt()));
    }

    if(cor.exec() == QDialog::Accepted) {
        QColor c = cor.currentColor().rgb();
        auto save = QString::number(c.red()) + "-" + QString::number(c.green()) + "-" + QString::number(c.blue());
        qDebug("%s(%sSubtitle%s)%s::%sAlterando a cor da legenda ...\033[0m", GRE, RED, GRE, RED, ORA);
        JsonTools::stringJson("sub_color", save);
        Q_EMIT changeColor(c);
    }
}


/** Setando uma fonte para a legenda */
void Sett::Subtitle::setFont() {
    bool ok;
    QFont current;
    if (QString::compare(JsonTools::stringJson("sub_font"), "undefined") != 0)
        current.fromString(JsonTools::stringJson("sub_font"));
    QFont font = QFontDialog::getFont(&ok, current);

    if (ok) {
        qDebug("%s(%sSubtitle%s)%s::%sAlterando a fonte da legenda ...\033[0m", GRE, RED, GRE, RED, ORA);
        JsonTools::stringJson("sub_font", font.toString());
        Q_EMIT changeFont(font);
    }
}


/** Ativando a legenda */
void Sett::Subtitle::setEnable(bool val) {
    QString at{"Desativando"};
    if (val) at = "Ativando";
    qDebug("%s(%sSubtitle%s)%s::%s%s a legenda ...\033[0m", GRE, RED, GRE, RED, ORA, STR(at));
    JsonTools::boolJson("sub_enable", val);
    Q_EMIT enableSub(val);
}


/** Ativando o auto carregamento da legenda */
void Sett::Subtitle::setAutoLoad(bool val) {
    QString at{"Desativando"};
    if (val) at = "Ativando";
    qDebug("%s(%sSubtitle%s)%s::%s%s o auto carregamento ...\033[0m", GRE, RED, GRE, RED, ORA, STR(at));
    JsonTools::boolJson("sub_autoload", val);
}


/** Alterando a engine da legenda */
void Sett::Subtitle::setEngine(const QString &engine) {
    qDebug("%s(%sSubtitle%s)%s::%sSelecionando a engine %s ...\033[0m", GRE, RED, GRE, RED, ORA, STR(engine));
    JsonTools::stringJson("sub_engine", engine);
    Q_EMIT changeEngine(engine);
}


/** Alterando o conjunto de caracteres da legenda */
void Sett::Subtitle::setCharset(const QString &charset) {
    qDebug("%s(%sSubtitle%s)%s::%sSelecionando o conjunto de caracteres %s ...\033[0m",
           GRE, RED, GRE, RED, ORA, STR(charset));
    JsonTools::stringJson("sub_charset", charset);
    Q_EMIT changeCharset();
}


/** Mudando o ícone nos estilos */
QString Sett::Subtitle::changeIconsStyle() {
    QString str{Utils::setStyle("checkbox")};
    QString theme{Utils::setThemeName()};

    QString ru{Utils::setIconTheme(theme, "checkbox-unselect")};
    if (!ru.isEmpty()) str.replace(QRegExp("/\\* _RADIO_UNCHECKED__ \\*/"), "image: url(" + ru + ");");

    QString rs{Utils::setIconTheme(theme, "checkbox-select")};
    if (!rs.isEmpty()) str.replace(QRegExp("/\\* _RADIO_CHECKED_ \\*/"), "image: url(" + rs + ");");

    if (ru.isEmpty() && rs.isEmpty()) return{};
    return str;
}


/** Mudando o tema dos ícones */
void Sett::Subtitle::changeIcons() {
    qDebug("%s(%sSubtitle%s)%s::%sAlterando ícones em Subtitle ...\033[0m", GRE, RED, GRE, RED, ORA);
    checkEnable->setStyleSheet(changeIconsStyle());
    checkAutoLoad->setStyleSheet(changeIconsStyle());
}

