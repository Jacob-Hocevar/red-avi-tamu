// TODO: add includes
#include "GUI_CTRL_Window.h"
#include "Frame_with_Title.h"
#include "Valves.h"

// For testing only
#include <QLabel>
#include <QFrame>
#include <QDebug>
#include<iostream>
using std::cout;
using std::endl;

// TODO: implement
GUI_CTRL_Window::GUI_CTRL_Window(GUI_Main_Window* parent, QSerialPort* ser):
    QWidget(), root(parent), ser(ser) {
    
    // Layout for the buttons and labels
    QGridLayout* bottom_layout = new QGridLayout();
    bottom_layout->setContentsMargins(5, 5, 5, 5);
    bottom_layout->setSpacing(10);

    // TODO: Delete, implement
    QLabel* tmp_label = new QLabel("<Put elements here>");
    bottom_layout->addWidget(tmp_label, 0, 0);

    Valve* test_valve = new Valve(ser, "V1", "Valve", 1);
    bottom_layout->addWidget(test_valve, 1, 0);
    Solenoid_Valve* test_Solenoid = new Solenoid_Valve(ser, "VS1", "Sol.", 1);
    bottom_layout->addWidget(test_Solenoid, 2, 0);
    LA_Ball_Valve* test_LA_BV = new LA_Ball_Valve(ser, "V4", "LA BV", 1, 0);
    bottom_layout->addWidget(test_LA_BV, 3, 0);

    QFrame* bottom_widget = new QFrame();
    bottom_widget->setLayout(bottom_layout);
    Frame_with_Title* layout = new Frame_with_Title("  Control Information", bottom_widget);

    this->setLayout(layout);
    this->root->add_to_main_window(this, 2, 0);
}

/*
    Public functions
*/

/*
    Private functions
*/
