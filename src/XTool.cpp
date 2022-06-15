#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#include "XTool.h"


/**********************************************************************************************************************/


/** Função que vai efetuar o clique */
void XTool::mouseClick() {
    Display *display = XOpenDisplay(nullptr);

    /** Pressionando o mouse */
    XTestFakeButtonEvent(display, 1, true, 0);
    XFlush(display);

    usleep(10000);
    
    /** Soltando o botão do mouse */
    XTestFakeButtonEvent(display, 1, false, 0);
    XFlush(display);

    XCloseDisplay(display);
}


/** Função que vai carregar o display para o clique */
void XTool::eventMouse(int x, int y) {
    Display *display = XOpenDisplay(nullptr);
    Window root = DefaultRootWindow(display);
    XTestFakeMotionEvent(display, int(root), x, y, 0);
    XFlush(display);
    mouseClick();
    XFlush(display);
    XCloseDisplay(display);
}
