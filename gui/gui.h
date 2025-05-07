#ifndef GUI_H
#define GUI_H

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>

class SimuladorGUI {
public:
    SimuladorGUI();
    void show();

private:
    Fl_Window *ventana;
    Fl_Button *btnSimA;
    Fl_Button *btnSimB;

    static void onClickSimA(Fl_Widget*, void*);
    static void onClickSimB(Fl_Widget*, void*);
};

#endif