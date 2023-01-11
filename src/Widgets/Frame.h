#ifndef OMPLAYER_FRAME_H
#define OMPLAYER_FRAME_H

#include <QFrame>

namespace Line {
    class Frame : public QFrame {
    Q_OBJECT

    public:
        enum ORIENTATION {Vertical = 0, Horizontal = 1};

        explicit Frame(Frame::ORIENTATION pos, int size = (-1));
        ~Frame() override;
    };
}
#endif //OMPLAYER_FRAME_H
