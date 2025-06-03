#include <QApplication>
#include "gui/mainmenu.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Crear y mostrar el menú principal en lugar de SimuladorGUI directamente
    MainMenu menu;
    menu.show();
    
    return app.exec();
}