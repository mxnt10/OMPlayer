#ifndef OMPLAYER_DECODER_H
#define OMPLAYER_DECODER_H

#include <QLayout>
#include <QToolButton>
#include <QSpinBox>
#include <QWidget>

#include <DecoderItemWidget>

class Decoder : public QWidget {
public:
    explicit Decoder(QWidget *parent = nullptr);
    ~Decoder() override;

    [[nodiscard]] QVariantHash videoDecoderOptions() const;
    QVector<QtAV::VideoDecoderId> decoderPriorityNames();
    void changeIcons();

private Q_SLOTS:
    static QVector<QtAV::VideoDecoderId> idsFromNames(const QStringList& names);
    void videoDecoderEnableChanged();

private:
    QList<DecoderItemWidget *> decItems{};
    QVBoxLayout *decLayout{};
    QStringList optionDecoder{};

};

#endif //OMPLAYER_DECODER_H
