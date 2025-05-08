#ifndef SIMULADORGUI_H
#define SIMULADORGUI_H

#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>

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
    QComboBox *comboAlgoritmo;
    QSpinBox *spinQuantum;
    QLabel *labelQuantum;
};

#endif