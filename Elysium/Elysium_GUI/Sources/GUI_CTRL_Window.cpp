// TODO: add includes
#include "GUI_CTRL_Window.h"
#include "Frame_with_Title.h"

#include <QDateTime>
#include <QLabel>
#include <QFrame>

// For testing only
#include <QDebug>
#include<iostream>
using std::cout;
using std::endl;

GUI_CTRL_Window::GUI_CTRL_Window(GUI_Main_Window* parent, QSerialPort* ser):
    QWidget(), root(parent), ser(ser), valves(new QList<Valve*>), is_saving(false), data_file(nullptr),
    start_save_btn(new QPushButton("Start Save")), end_save_btn(new QPushButton("End Save")) {
    
    // Layout for the buttons and labels
    QGridLayout* bottom_layout = new QGridLayout();
    bottom_layout->setContentsMargins(5, 5, 5, 5);
    bottom_layout->setSpacing(10);

    // Open the configuration file and add the listed valves
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

            QObject::connect(valve, SIGNAL(updated_state(const QString&)), this, SLOT(save(const QString&)));

            // Add valve to list and to layout;
            this->valves->append(valve);
            bottom_layout->addWidget(valve, i++, 0);
        }
    }

    // Configure the start/end save buttons
    this->end_save_btn->setDisabled(true);
    QObject::connect(this->start_save_btn, SIGNAL(clicked()), this, SLOT(start_save()));
    QObject::connect(this->end_save_btn, SIGNAL(clicked()), this, SLOT(end_save()));
    bottom_layout->addWidget(this->start_save_btn, 0, 10);
    bottom_layout->addWidget(this->end_save_btn, 1, 10);

    // TODO: Implement Control State

    QFrame* bottom_widget = new QFrame();
    bottom_widget->setLayout(bottom_layout);
    Frame_with_Title* layout = new Frame_with_Title("Control Information", bottom_widget);

    this->setLayout(layout);
    this->root->add_to_main_window(this, 2, 0);
}

void GUI_CTRL_Window::start_save() {
    // Create the "Data" directory, if needed
    QDir data_dir("Data");
    if (!data_dir.exists()) {
        data_dir = QDir(".");
        data_dir.mkdir("Data");
    }

    // Name the file after the configuration, all indexed, starting at 0, as a .csv
    QString file_name = "Data/" + root->get_configuration()->dirName().split('.')[0] + "_CTRL_test_";
    int test_num = 0;
    this->data_file = new QFile(file_name + QString::number(test_num) + ".csv");

    // Loop through all numbers until the smallest index that does not already exist is found
    while (this->data_file->exists()) {
        this->data_file->setFileName(file_name + QString::number(++test_num) + ".csv");
    }

    // Next, create the file
    // If it fails to create, output an error message and abort
    if (!this->data_file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        cout << "Could not create file: " << this->data_file->fileName().toStdString() << endl;
        return;
    }

    // Otherwise, set the is_saving variable, flip active buttons
    this->is_saving = true;
    this->start_save_btn->setDisabled(true);
    this->end_save_btn->setEnabled(true);

    // Write the header for the .csv file
    QTextStream out(this->data_file);
    out << "Global Time (ms since Epoch)";
    for (int i = 0; i < this->valves->size(); ++i) {
        out << ',' << this->valves->at(i)->get_ID() << " State (1=Open)";
    }
    out << ",Control State,Last Command" << endl;

    // Write the first line (command is "Start Save")
    this->save("Start Save");
}

void GUI_CTRL_Window::end_save() {
    // Close file, update internal variable, flip active buttons
    this->data_file->close();
    this->is_saving = false;
    this->start_save_btn->setEnabled(true);
    this->end_save_btn->setDisabled(true);
}

void GUI_CTRL_Window::save(const QString& command) {
    // Do not attempt to save anything if not saving
    if (!is_saving) {
        return;
    }

    // Handles outputs via a text stream for nicer syntax and control with QStrings
    QTextStream out(this->data_file);
    out << QDateTime::currentMSecsSinceEpoch();
    for (int i = 0; i < this->valves->size(); ++i) {
        out << ',' << this->valves->at(i)->get_state();
    }
    // TODO: Implement control state
    out << ",," << command << endl;
}
