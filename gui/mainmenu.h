#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include "gui.h"

class MainMenu : public QMainWindow {
    Q_OBJECT

public:
    explicit MainMenu(QWidget *parent = nullptr);

private slots:
    void onSimuladorVisualClicked();
    void onCalcularEstadisticasClicked();

private:
    QPushButton *btnSimuladorVisual;
    QPushButton *btnCalcularEstadisticas;
    QLabel *lblTitulo;
    
    SimuladorGUI *simuladorWindow;
    QWidget *estadisticasWindow; 
};

#endif 