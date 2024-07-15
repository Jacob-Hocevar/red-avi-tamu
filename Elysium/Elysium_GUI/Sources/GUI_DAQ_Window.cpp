// TODO: add includes
#include "GUI_DAQ_Window.h"
#include "Frame_with_Title.h"

// For testing only
#include <QLabel>
#include <QFrame>
#include <QDebug>
#include<iostream>
using std::cout;
using std::endl;

// TODO: implement
GUI_DAQ_Window::GUI_DAQ_Window(GUI_Main_Window* parent, QSerialPort* ser):
    QWidget(), root(parent), ser(ser) {
    
    // Layout for the buttons and labels
    QGridLayout* bottom_layout = new QGridLayout();
    bottom_layout->setContentsMargins(5, 5, 5, 5);
    bottom_layout->setSpacing(10);

    // TODO: Delete, implement
    QLabel* tmp_label = new QLabel("<Put elements here>");
    bottom_layout->addWidget(tmp_label, 0, 0);

    QFrame* bottom_widget = new QFrame();
    bottom_widget->setLayout(bottom_layout);
    Frame_with_Title* layout = new Frame_with_Title("  Sensor Information", bottom_widget);

    this->setLayout(layout);
    this->root->add_to_main_window(this, 3, 0);
}

/*
    Public functions
*/

void GUI_DAQ_Window::start() {

}

/*
    Private functions
*/
