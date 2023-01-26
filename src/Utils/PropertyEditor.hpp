#ifndef OMPLAYER_PROPERTYEDITOR_HPP
#define OMPLAYER_PROPERTYEDITOR_HPP

#include <QAction>
#include <QObject>
#include <QMetaProperty>
#include <QMetaEnum>

class PropertyEditor : public QObject {
Q_OBJECT

public:
    explicit PropertyEditor(QObject *parent = nullptr);
    void getProperties(QObject *obj);
    QWidget* buildUi(QObject* obj = nullptr);
    QVariantHash exportAsHash() { return properties; };

private:
    QWidget* createWidgetForFlags(const QString& name,
                                  const QVariant& value,
                                  QMetaEnum me,
                                  const QString& detail = QString(),
                                  QWidget* parent = nullptr);

    QWidget* createWidgetForEnum(const QString& name,
                                 const QVariant& value,
                                 QMetaEnum me,
                                 const QString& detail = QString(),
                                 QWidget* parent = nullptr);

    QWidget* createWidgetForInt(const QString& name,
                                int value,
                                const QString& detail = QString(),
                                QWidget* parent = nullptr);

    QWidget* createWidgetForText(const QString& name,
                                 const QString& value,
                                 bool readOnly,
                                 const QString& detail = QString(),
                                 QWidget* parent = nullptr);

    QWidget* createWidgetForBool(const QString& name,
                                 bool value,
                                 const QString& detail = QString(),
                                 QWidget* parent = nullptr);

    void updatePropertyValue(const QString& name, const QVariant& value);

private Q_SLOTS:
    void onFlagChange(QAction *action);
    void onEnumChange(int value);
    void onIntChange(int value);
    void onTextChange(const QString& value);
    void onBoolChange(bool value);

private:
    QList<QMetaProperty> metaProperties;
    QVariantHash properties;
    QVariantHash propertyDetails;
};

#endif // OMPLAYER_PROPERTYEDITOR_HPP
