#include "GUI_DAQ_Window.h"
#include "Frame_with_Title.h"
#include "Sensor.h"

#include <QTextStream>
#include <QStringList>
#include <QLabel>
#include <QFrame>
#include <QFile>
#include <QHash>

// For testing only
#include <QDebug>
#include<iostream>
using std::cout;
using std::endl;

GUI_DAQ_Window::GUI_DAQ_Window(GUI_Main_Window* parent, QSerialPort* ser):
    QWidget(), root(parent), ser(ser) {
    
    // Layout for the buttons and labels
    QGridLayout* bottom_layout = new QGridLayout();
    bottom_layout->setContentsMargins(5, 5, 5, 5);
    bottom_layout->setSpacing(10);

    QHash<QString, Sensor*> sensors;
    QFile file(root->get_configuration()->filePath("sensors.cfg"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);

        int i = 0;
        while (!in.atEnd()) {
            QStringList info = in.readLine().split(',');
            sensors[info.at(0)] = new Sensor(info.at(0), info.at(1), info.at(2));
            bottom_layout->addWidget(sensors[info.at(0)], i, 0);
            ++i;
        }

    } else {
        cout << "Cannot open file" << endl;
    }


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
