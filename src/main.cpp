#include "../gui/gui.h"
#include <FL/Fl.H>

int main(int argc, char **argv) {
    SimuladorGUI gui;
    gui.show();
    return Fl::run();
}