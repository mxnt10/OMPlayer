#include <QLayout>
#include <QPainter>
#include <QTimer>
#include <Button>
#include <Utils>

#include "DecoderItemWidget.h"


/**********************************************************************************************************************/


/** Construtor */
DecoderItemWidget::DecoderItemWidget(QWidget *parent): QFrame(parent) {
    setLineWidth(2);
    editor = new PropertyEditor();


    /** Criando uma linha */
    auto *frame = new QFrame();
    frame->setFrameShape(QFrame::HLine);


    /** Botão de expansão */
    expandBtn = new Button(Button::Default, 28, "add");
    connect(expandBtn, &Button::clicked, this, &DecoderItemWidget::toggleEditorVisible);


    /** CheckBox para os itens */
    check = new QCheckBox();
    check->setFocusPolicy(Qt::NoFocus);
    check->setStyleSheet(changeIconsStyle());
    connect(check, &QCheckBox::pressed, this, &DecoderItemWidget::checkPressed);


    /** Layout dos itens*/
    auto *hb = new QHBoxLayout();
    hb->setSpacing(0);
    hb->addWidget(check);
    hb->addWidget(expandBtn);


    /** Criação do layout */
    auto *vb = new QVBoxLayout();
    vb->addWidget(frame);
    vb->addLayout(hb);
    setLayout(vb);
}


/**********************************************************************************************************************/


/** Constrói uma interface para o objeto */
void DecoderItemWidget::buildUiFor(QObject *obj) {
    editor->getProperties(obj);
    QWidget *w = editor->buildUi(obj);
    if (!w) return;

    /** Adicionando o widget ao layout de main */
    editorWidget = w;
    w->setEnabled(true);
    layout()->addWidget(w);
    w->setVisible(false);
}


/** Alterando o ícone dos botões e alterando a visualização das opções */
void DecoderItemWidget::toggleEditorVisible() {
    if (!editorWidget) return;
    editorWidget->setVisible(!editorWidget->isVisible());

    QString name{"add"};
    if (editorWidget->isVisible()) name = "remove";
    iconName = name;

    Utils::changeIcon(expandBtn, name);
    parentWidget()->adjustSize();
}


/** Emissão ao pressionar o checkbox */
void DecoderItemWidget::checkPressed() { QTimer::singleShot(500, [this](){ Q_EMIT enableChanged(); }); }


/** Alterando o ícone dos botões */
void DecoderItemWidget::changeIcons() {
    QString name{"add"};
    if (iconName == "remove") name = "remove";
    Utils::changeIcon(expandBtn, name);
    check->setStyleSheet(changeIconsStyle());
}


/** Mudando o ícone nos estilos */
QString DecoderItemWidget::changeIconsStyle() {
    QString str{Utils::setStyle("checkbox")};
    QString theme{Utils::setThemeName()};

    QString ru{Utils::setIconTheme(theme, "checkbox-unselect")};
    if (!ru.isEmpty()) str.replace(QRegExp("/\\* _RADIO_UNCHECKED__ \\*/"), "image: url(" + ru + ");");

    QString rs{Utils::setIconTheme(theme, "checkbox-select")};
    if (!rs.isEmpty()) str.replace(QRegExp("/\\* _RADIO_CHECKED_ \\*/"), "image: url(" + rs + ");");

    if (ru.isEmpty() && rs.isEmpty()) return{};
    return str;
}
