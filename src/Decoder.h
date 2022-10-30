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

};

#endif //OMPLAYER_DECODER_H
