#include "GUI_DAQ_Window.h"
#include "Frame_with_Title.h"
#include "GUI_Graph_Window.h"
#include "Sensor.h"

#include <QTextStream>
#include <QDateTime>
#include <QLabel>
#include <QFrame>
#include <QFile>
#include <QHash>

// For testing only
#include <QDebug>
#include<iostream>
using std::cout;
using std::endl;

// Number of milliseconds to notify the user that no new data has arrived
const int CHECK_CONNECTION_INTERVAL = 100;

GUI_DAQ_Window::GUI_DAQ_Window(GUI_Main_Window* parent, QSerialPort* ser):
    QWidget(), root(parent), ser(ser), sensors(), derived_IDs(), is_saving(false), data_file(nullptr),
    start_save_btn(new QPushButton("Start Save")), end_save_btn(new QPushButton("End Save")),
    check_connection_timer(new QTimer(this)), graphs(nullptr) {
    
    // Layout for the buttons and labels
    QGridLayout* bottom_layout = new QGridLayout();
    bottom_layout->setContentsMargins(5, 5, 5, 5);
    bottom_layout->setSpacing(10);

    // TODO: Implement local storage of data
    // Possibly a rolling window of 20 data points, take the average every 5
    // Then store the last 200 averaged points for plotting
    // OR: Do it based on time
    // Likely, use another QHash, with key = ID and data type as lists of doubles

    // Construct a hashmap of the sensors from the configuration file,
    // so that they can be dynamically generated and accessed
    QFile file(root->get_configuration()->filePath("sensors.cfg"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);

        int i = 0;
        while (!in.atEnd()) {
            // Read the first line which contains Sensor initialization info
            QStringList info = in.readLine().split(',');
            QString ID = info.at(0);
            this->sensors[ID] = new Sensor(ID, info.at(1), info.at(2));

            // Read the second line which contains placement and type info
            // TODO: Implement groups
            info = in.readLine().split(',');
            bottom_layout->addWidget(this->sensors[ID], i, 0);
            
            if ("derived" == info.at(1)) {
                this->derived_IDs.append(ID);
            }
            ++i;
        }

    } else {
        cout << "Cannot open file" << endl;
    }

    // Configure the start/end save buttons
    this->end_save_btn->setDisabled(true);
    QObject::connect(this->start_save_btn, SIGNAL(clicked()), this, SLOT(start_save()));
    QObject::connect(this->end_save_btn, SIGNAL(clicked()), this, SLOT(end_save()));
    bottom_layout->addWidget(this->start_save_btn, 0, 10, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
    bottom_layout->addWidget(this->end_save_btn, 1, 10, 1, 1, Qt::AlignRight | Qt::AlignVCenter);

    // Build frame and title for the Sensor section
    QFrame* bottom_widget = new QFrame();
    bottom_widget->setLayout(bottom_layout);
    Frame_with_Title* layout = new Frame_with_Title("Sensor Information", bottom_widget);

    // Attach to the main window
    this->setLayout(layout);
    this->root->add_to_main_window(this, 3, 0);

    // Connect the serial connection to the private slot so that it automatically updates the sensors
    QObject::connect(this->ser, SIGNAL(readyRead()), this, SLOT(update_sensors()));

    // If new data is not recieved within CHECK_CONNECTION_INTERVAL, notify the user
    QObject::connect(this->check_connection_timer, SIGNAL(timeout()), this, SLOT(connection_failed()));
    this->check_connection_timer->start(CHECK_CONNECTION_INTERVAL);

    // Create graphs module
    graphs = new GUI_Graph_Window(this->root, this);
}

GUI_DAQ_Window::~GUI_DAQ_Window() {
    if (this->graphs) {
        delete graphs;
    }
}

void GUI_DAQ_Window::update_sensors() {
    // Text streams are far easier to handle than the raw data stream (auto converts to QString, nicer readLine())
    QTextStream in(this->ser->readAll());
    while (!in.atEnd()) {
        QStringList info = in.readLine().split(',');

        // Examine each sensor separately
        for (int i = 0; i < info.size(); ++i) {
            // Isolate key, value pairs
            QStringList cur_info = info.at(i).split(':');

            if (cur_info.size() < 2) {
                cout << "Not enough data: " << info.at(i).toStdString() << endl;
                continue;
            } if (!sensors.contains(cur_info.at(0))) {
                cout << "No sensor with ID: " << cur_info.at(0).toStdString() << endl;
                continue;
            }

            // Update sensors
            sensors[cur_info.at(0)]->update_data(cur_info.at(1));
        }
    }

    for (int i = 0; i < this->derived_IDs.size(); ++i) {
        this->update_derived(this->derived_IDs.at(i));
    }

    // Every time the sensors are updated, attempt to record the data
    // The check for if saving is enabled is handled inside save()
    this->save();
}

void GUI_DAQ_Window::update_derived(const QString& ID) {
    double new_data;
    if ("mfr" == ID) {
        // TODO: Implement with correct equation and DAQ data stores
        double P1 = this->sensors["P1"]->get_data().toDouble();
        double P2 = 20;
        double new_mfr = (P2 - P1) * 1.0234;
        new_data = new_mfr;
    } else if ("T_total" == ID) {
        new_data = -1.0;
    } else {
        cout << "No Known Calculation for Derived Quantity: " << ID.toStdString() << endl;
        return;
    }
    sensors[ID]->update_data(QString::number(new_data));
}

void GUI_DAQ_Window::start_save() {
    // Create the "Data" directory, if needed
    QDir data_dir("Data");
    if (!data_dir.exists()) {
        data_dir = QDir(".");
        data_dir.mkdir("Data");
    }

    // Name the file after the configuration, all indexed, starting at 0, as a .csv
    QString file_name = "Data/" + root->get_configuration()->dirName().split('.')[0] + "_DAQ_test_";
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
    out << "Global Time (ns from Epoch),Local Time (ms from Teensy boot)";
    QStringList keys = this->sensors.keys();
    for (int i = 0; i < keys.size(); ++i) {
        out << ',' << this->sensors[keys[i]]->get_full_name();
    }
    out << endl;
}

void GUI_DAQ_Window::end_save() {
    // Close file, update internal variable, flip active buttons
    this->data_file->close();
    this->is_saving = false;
    this->start_save_btn->setEnabled(true);
    this->end_save_btn->setDisabled(true);
}

void GUI_DAQ_Window::save() {
    // Reset the timer checking if it is recieving data
    this->check_connection_timer->start();

    // Do not attempt to save anything if not saving
    if (!is_saving) {
        return;
    }

    // Handles outputs via a text stream for nicer syntax and control with QStrings
    QTextStream out(this->data_file);
    out << QDateTime::currentMSecsSinceEpoch();

    // TODO: Local time
    out << ',';

    QStringList keys = this->sensors.keys();
    for (int i = 0; i < keys.size(); ++i) {
        // TODO: Use locally stored sensor data (possibly, the averaged values)
        // instead of reading from the sensor object (note that this is also locally stored, not a serial read)
        out << ',' << this->sensors[keys[i]]->get_data();
    }
    out << endl;
}

void GUI_DAQ_Window::connection_failed() {
    cout << "Connection failed" << endl;
}
