#ifndef OMPLAYER_DECODERITEMWIDGET_H
#define OMPLAYER_DECODERITEMWIDGET_H

#include <QCheckBox>
#include <QFrame>
#include <QLabel>
#include <PropertyEditor>

class DecoderItemWidget : public QFrame {
Q_OBJECT

public:
    explicit DecoderItemWidget(QWidget* parent = nullptr);
    void buildUiFor(QObject *obj);

    [[nodiscard]] QVariantHash getOptions() const { return editor->exportAsHash(); }
    [[nodiscard]] bool isChecked() const { return check->isChecked(); }
    [[nodiscard]] QString name() const { return check->text();}
    [[nodiscard]] QString description() const { return desc->text();}
    void setChecked(bool c) { check->setChecked(c); }
    void setName(const QString& name) { check->setText(name);}
    void setDescription(const QString& des) { desc->setText(des); }

Q_SIGNALS:
    void enableChanged();

private Q_SLOTS:
    void checkPressed();
    void toggleEditorVisible();

private:
    QCheckBox *check;
    QLabel *desc;
    PropertyEditor *editor{};
    QWidget *editorWidget{nullptr};
};

#endif //OMPLAYER_DECODERITEMWIDGET_H
