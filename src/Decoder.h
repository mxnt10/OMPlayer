#ifndef OMPLAYER_DECODER_H
#define OMPLAYER_DECODER_H

#include <QWidget>

class Decoder : public QWidget {
public:
    explicit Decoder(QWidget *parent = nullptr);
    ~Decoder() override;

};

#endif //OMPLAYER_DECODER_H
