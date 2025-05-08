#ifndef SIMULADORGUI_H
#define SIMULADORGUI_H

#include <QMainWindow>
#include <QPushButton>

class SimuladorGUI : public QMainWindow {
    Q_OBJECT

public:
    SimuladorGUI(QWidget *parent = nullptr);

private slots:
    void onSimulacionAClicked();
    void onSimulacionBClicked();

private:
    QPushButton *btnSimA;
    QPushButton *btnSimB;
};

#endif