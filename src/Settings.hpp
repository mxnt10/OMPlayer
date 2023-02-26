#ifndef OMPLAYER_SETTINGS_HPP
#define OMPLAYER_SETTINGS_HPP

#include <QtAV>
#include <QtAVWidgets>
#include <QRadioButton>
#include <QTabWidget>
#include <Button>
#include <Dialog>
#include <Settings>

class Settings : public Dialog {
Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings() override;
    [[nodiscard]] QVariantHash videoDecoderOptions() const;
    QVector<QtAV::VideoDecoderId> decoderPriorityNames();


#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"

Q_SIGNALS:
    void onclose();
    void emitvalue(const QString &value);
    void emitcolor(QColor color);
    void emitfont(QFont font);
    void emitEnableSub(bool val);
    void emitEngine(const QString &engine);
    void emitCharset();
    void changethemeicon();

#pragma clang diagnostic pop


private Q_SLOTS:
    void setIcon(const QString &index);

private:
    QTabWidget *tab{};
    Button *closebtn{};
    Decoder *decoder{};
    Sett::Subtitle *subtitle{};
    Rend::Renderer *renderer{};
};

#endif //OMPLAYER_SETTINGS_HPP
