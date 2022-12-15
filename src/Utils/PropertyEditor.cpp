#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMenu>
#include <QSpinBox>
#include <QToolButton>

#include "ClickableMenu.h"
#include "PropertyEditor.h"
#include "Utils.h"


/**********************************************************************************************************************/


/** Construtor */
PropertyEditor::PropertyEditor(QObject *parent) : QObject(parent) {}


/** Setando e analisando propriedades de um objeto */
void PropertyEditor::getProperties(QObject *obj) {
    metaProperties.clear();
    properties.clear();
    propertyDetails.clear();

    if (!obj) return;
    const QMetaObject *mo = obj->metaObject();

    for (int i = 0; i < mo->propertyCount(); ++i) {
        QMetaProperty mp = mo->property(i);
        metaProperties.append(mp);
        QVariant v(mp.read(obj));

        if (mp.isEnumType()) properties.insert(QString::fromLatin1(mp.name()), v.toInt());
        else properties.insert(QString::fromLatin1(mp.name()), v);

        const QVariant detail = obj->property(QByteArray("detail_").append(mp.name()).constData());
        if (!detail.isNull()) propertyDetails.insert(QString::fromLatin1(mp.name()), detail.toString());
    }
    properties.remove(QString::fromLatin1("objectName"));
}


/** Criando os itens para ser adicionado a interface */
QWidget* PropertyEditor::buildUi(QObject *obj) {
    if (metaProperties.isEmpty()) return{};
    auto *w = new QWidget();
    auto *gl = new QGridLayout();
    w->setLayout(gl);
    int row = 0;
    QVariant value;

    foreach (QMetaProperty mp, metaProperties) {
        if (qstrcmp(mp.name(), "objectName") == 0) continue;
        value = properties[QString::fromLatin1(mp.name())];
        if (mp.isEnumType()) {
            if (mp.isFlagType()) {
                gl->addWidget(createWidgetForFlags(
                        QString::fromLatin1(mp.name()), value, mp.enumerator(),
                        obj ? obj->property(QByteArray("detail_").append(mp.name()).constData()).toString() :
                        QString()), row, 0, Qt::AlignLeft | Qt::AlignVCenter);
            } else {
                gl->addWidget(new QLabel(mp.name()), row, 0, Qt::AlignRight | Qt::AlignVCenter);
                gl->addWidget(createWidgetForEnum(
                        QString::fromLatin1(mp.name()), value, mp.enumerator(),
                        obj ? obj->property(QByteArray("detail_").append(mp.name()).constData()).toString() :
                        QString()), row, 1, Qt::AlignLeft | Qt::AlignVCenter);
            }
        } else if ( mp.type() == QVariant::Int || mp.type() == QVariant::UInt ||
                    mp.type() == QVariant::LongLong || mp.type() == QVariant::ULongLong ) {
            gl->addWidget(new QLabel(mp.name()), row, 0, Qt::AlignRight | Qt::AlignVCenter);
            gl->addWidget(createWidgetForInt(
                    QString::fromLatin1(mp.name()), value.toInt(),
                    obj ? obj->property(QByteArray("detail_").append(mp.name()).constData()).toString() : QString()),
                          row, 1, Qt::AlignLeft | Qt::AlignVCenter);
        } else if (mp.type() == QVariant::Bool) {
            gl->addWidget(createWidgetForBool(
                    QString::fromLatin1(mp.name()), value.toBool(),
                    obj ? obj->property(QByteArray("detail_").append(mp.name()).constData()).toString() : QString()),
                          row, 0, 1, 2, Qt::AlignLeft);
        } else {
            gl->addWidget(new QLabel(mp.name()), row, 0, Qt::AlignRight | Qt::AlignVCenter);
            gl->addWidget(createWidgetForText(
                    QString::fromLatin1(mp.name()), value.toString(), !mp.isWritable(),
                    obj ? obj->property(QByteArray("detail_").append(mp.name()).constData()).toString() : QString()),
                          row, 1, Qt::AlignLeft | Qt::AlignVCenter);
        }
        ++row;
    }
    return w;
}


/** Criando widget para menu */
QWidget* PropertyEditor::createWidgetForFlags(const QString& name, const QVariant& value, QMetaEnum me,
                                              const QString &detail, QWidget* parent) {
    properties[name] = value;

    auto *btn = new QToolButton(parent);
    if (!detail.isEmpty()) btn->setToolTip(detail);
    btn->setObjectName(name);
    btn->setText(name.toUtf8().constData());
    btn->setPopupMode(QToolButton::InstantPopup);

    auto *menu = new ClickableMenu(nullptr, btn);
    menu->setStyleSheet(Utils::setStyle("tabmenu"));
    menu->setWindowOpacity(1);
    menu->setObjectName(name);
    btn->setMenu(menu);

    for (int i = 0; i < me.keyCount(); ++i) {
        QAction * a = menu->addAction(QString::fromLatin1(me.key(i)));
        a->setCheckable(true);
        a->setData(me.value(i));
        a->setChecked(value.toInt() & me.value(i));
    }

    connect(menu, &QMenu::triggered, this, &PropertyEditor::onFlagChange);
    return btn;
}


/** Criando widget para combobox */
QWidget* PropertyEditor::createWidgetForEnum(const QString& name, const QVariant& value, QMetaEnum me,
                                             const QString &detail, QWidget* parent) {
    properties[name] = value;

    auto *box = new QComboBox(parent);
    if (!detail.isEmpty()) box->setToolTip(detail);
    box->setObjectName(name);
    box->setEditable(false);
    box->setStyleSheet(Utils::setStyle("global"));

    for (int i = 0; i < me.keyCount(); ++i) box->addItem(QString::fromLatin1(me.key(i)), me.value(i));

    if (value.type() == QVariant::Int)box->setCurrentIndex(box->findData(value));
    else box->setCurrentText(value.toString());

    connect(box, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PropertyEditor::onEnumChange);
    return box;
}


/** Criando widget para spinbox */
QWidget* PropertyEditor::createWidgetForInt(const QString& name, int value, const QString& detail, QWidget* parent) {
    properties[name] = value;

    auto *box = new QSpinBox(parent);
    if (!detail.isEmpty()) box->setToolTip(detail);
    box->setObjectName(name);
    box->setValue(value);
    box->setStyleSheet(Utils::setStyle("global"));

    connect(box, QOverload<int>::of(&QSpinBox::valueChanged), this, &PropertyEditor::onIntChange);
    return box;
}


/** Criando widget para label */
QWidget* PropertyEditor::createWidgetForText(const QString& name, const QString& value, bool readOnly,
                                             const QString& detail, QWidget* parent) {
    properties[name] = value;

    QWidget *w;
    if (readOnly) {
        auto *label = new QLabel(parent);
        w = label;
        label->setText(value);
    } else {
        auto *box = new QLineEdit(parent);
        w = box;
        box->setText(value);
        connect(box, &QLineEdit::textChanged, this, &PropertyEditor::onTextChange);
    }

    if (!detail.isEmpty()) w->setToolTip(detail);
    w->setObjectName(name);
    return w;
}


/** Criando widget para checkbox */
QWidget* PropertyEditor::createWidgetForBool(const QString& name, bool value, const QString &detail, QWidget* parent) {
    properties[name] = value;

    auto *box = new QCheckBox(name.toUtf8().constData(), parent);
    if (!detail.isEmpty()) box->setToolTip(detail);
    box->setObjectName(name);
    box->setChecked(value);
    box->setFocusPolicy(Qt::NoFocus);
    box->setStyleSheet(Utils::setStyle("global"));

    connect(box, &QCheckBox::clicked, this, &PropertyEditor::onBoolChange);
    return box;
}


/** Ações se o valor dos widgets forem alterados */
void PropertyEditor::updatePropertyValue(const QString &name, const QVariant &value) {
    if (name.isEmpty()) return;
    if (!properties.contains(name)) return;

    qDebug("%s(%sPropertyEditor%s)%s::%sSetando configuração: %s%s%s >>> %s%s\033[0m",
           GRE, RED, GRE, RED, BLU, GRE, STR(name), RDL, YEL, STR(value.toString()));
    properties[name] = value;
}


/** Envia um action para o updatePropertyValue */
void PropertyEditor::onFlagChange(QAction *action) {
    int value = properties[sender()->objectName()].toInt();
    int flag = action->data().toInt();

    if (action->isChecked()) value |= flag;
    else value &= ~flag;

    updatePropertyValue(sender()->objectName(), value);
}


/** Envia um item combobox para o updatePropertyValue */
void PropertyEditor::onEnumChange(int value) {
    auto *box =  qobject_cast<QComboBox*>(sender());
    updatePropertyValue(sender()->objectName(), box->itemData(value));
}


/** Envia um int para o updatePropertyValue */
void PropertyEditor::onIntChange(int value) { updatePropertyValue(sender()->objectName(), value); }


/** Envia uma string para o updatePropertyValue */
void PropertyEditor::onTextChange(const QString& value) { updatePropertyValue(sender()->objectName(), value); }


/** Envia um valor booleano para o updatePropertyValue */
void PropertyEditor::onBoolChange(bool value) { updatePropertyValue(sender()->objectName(), value); }


/** Função que retorna as configurações de decodificação */
QVariantHash PropertyEditor::exportAsHash() { return properties; }
