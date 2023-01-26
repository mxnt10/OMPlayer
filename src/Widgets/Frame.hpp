#ifndef OMPLAYER_FRAME_HPP
#define OMPLAYER_FRAME_HPP

#include <QFrame>
#define null (-1)

namespace Line {
    class Frame : public QFrame {
    Q_OBJECT

    public:
        enum Orientation {Vertical = 0, Horizontal = 1};

        explicit Frame(Frame::Orientation pos, int size = null);
        ~Frame() override;
    };
}

#endif //OMPLAYER_FRAME_HPP
