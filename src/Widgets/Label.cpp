#include <QDebug>
#include <Utils>

#include "Label.hpp"


/**********************************************************************************************************************/


/** Contrutor para o Label */
Label::Label(const QFlag& align, int w, const QString& text) {
    setAlignment(align);
    if (w > null) setFixedWidth(w);
    if (!text.isEmpty()) setText(text);
}


/** Contrutor com parent */
Label::Label(const QFlag &align, int w, int h, QWidget *parent) : QLabel(parent) {
    setAlignment(align);
    if (w > null) setFixedWidth(w);
    if (h > null) setFixedHeight(h);
}


/** Construtor para a imagem */
Label::Label(const QFlag& align, const QString &local) {
    setAlignment(align);
    setPixmap(QPixmap(local).scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}


/** Construtor com atributos */
Label::Label(QWidget *parent) : QLabel(parent) {
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
}


/** Destrutor */
Label::~Label() = default;


/**********************************************************************************************************************/


/** Construtor EmptyList */
EmptyList::EmptyList(const QString &text, QWidget *parent) : QLabel(text, parent){

    /** Paleta de cor */
    QPalette palete;
    palete.setColor(QPalette::WindowText, QColor(150, 150, 150));

    /** Fonte do texto */
    QFont font;
    font.setItalic(true);
    font.setPixelSize(16);

    /** Definindo propriedades */
    setFont(font);
    setPalette(palete);
}
