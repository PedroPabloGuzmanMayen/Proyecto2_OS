#include <QApplication>
#include "gui/gui.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    SimuladorGUI ventana;
    ventana.show();
    return app.exec();
}