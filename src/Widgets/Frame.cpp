#include "Frame.hpp"


/**********************************************************************************************************************/


/** Construtor para definir as propriedades */
Line::Frame::Frame(Frame::Orientation pos, int size) : QFrame() {
    setFrameShadow(QFrame::Sunken);
    setStyleSheet("background: #cccccc");

    if (pos == Frame::Vertical) {
        setFixedWidth(2);
        if (size > null) setFixedHeight(size);
    } else if (pos == Frame::Horizontal) {
        setFixedHeight(2);
        if (size > null) setFixedWidth(size);
    }
}


/** Destrutor */
Line::Frame::~Frame() = default;

