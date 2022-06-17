#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>


/**********************************************************************************************************************/


/** Função que vai efetuar o clique */
void mouseClick() {
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
