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
    QFile file(root->get_configuration()->filePath("valves.cfg"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);

        int i = 0;
        while (!in.atEnd()) {
            QStringList info = in.readLine().split(',');
            Valve* valve = nullptr;
            if ("Valve" == info[0]) {
                valve = new Valve(ser, info[1], info[2], info[3].toInt());
            } else if ("Solenoid" == info[0]) {
                valve = new Solenoid_Valve(ser, info[1], info[2], info[3].toInt());
            } else if ("LA_Ball" == info[0]) {
                valve = new LA_Ball_Valve(ser, info[1], info[2], info[3].toInt(), info[4].toInt());
            } else {
                cout << "INVALID FORMAT in valves.cfg" << endl;
            }
            bottom_layout->addWidget(valve, i, 0);
            ++i;
        }
    }

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
