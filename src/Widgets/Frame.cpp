#include "Frame.h"


/**********************************************************************************************************************/


/** Construtor para definir as propriedades */
Line::Frame::Frame(Frame::ORIENTATION pos, int size) : QFrame() {
    setFrameShadow(QFrame::Sunken);
    setStyleSheet("background: #cccccc");

    if (pos == Frame::Vertical) {
        setFixedWidth(2);
        if (size > (-1)) setFixedHeight(size);
    } else if (pos == Frame::Horizontal) {
        setFixedHeight(2);
        if (size > (-1)) setFixedWidth(size);
    }
}


/** Destrutor */
Line::Frame::~Frame() = default;

