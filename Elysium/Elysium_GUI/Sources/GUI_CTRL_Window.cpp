// TODO: add includes
#include "GUI_CTRL_Window.h"
#include "Frame_with_Title.h"
#include "Standard_Label.h"

#include <QTabWidget>
#include <QDateTime>
#include <QLabel>
#include <QFrame>

// For testing only
#include <QDebug>
#include<iostream>
using std::cout;
using std::endl;

GUI_CTRL_Window::GUI_CTRL_Window(GUI_Main_Window* parent, QSerialPort* ser):
    QWidget(), root(parent), ser(ser), valves(new QList<Valve*>), control_state(new QLabel("-")), control_states(),
    is_saving(false), data_file(nullptr), start_save_btn(new QPushButton("Start Save")),
    end_save_btn(new QPushButton("End Save")) {
    
    // Layout for the individual valve control setup
    QGridLayout* valve_layout = new QGridLayout();
    valve_layout->setContentsMargins(5, 5, 5, 5);
    valve_layout->setSpacing(10);

    // Layout for the operation/control state setup
    QGridLayout* operation_layout = new QGridLayout();
    operation_layout->setContentsMargins(5, 5, 5, 5);
    operation_layout->setSpacing(10);

    // Open the valves configuration file and add the listed valves
    QFile valve_file(root->get_configuration()->filePath("valves.cfg"));
    if (valve_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&valve_file);

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
            valve_layout->addWidget(valve, i++, 0, 1, 2);
        }
    }

    // Configure the start/end save buttons
    this->end_save_btn->setDisabled(true);
    QObject::connect(this->start_save_btn, SIGNAL(clicked()), this, SLOT(start_save()));
    QObject::connect(this->end_save_btn, SIGNAL(clicked()), this, SLOT(end_save()));

    // Make a group of the aligned save buttons
    QGridLayout* save_btns_layout = new QGridLayout();
    save_btns_layout->addWidget(this->start_save_btn, 0, 1);
    save_btns_layout->addWidget(this->end_save_btn, 1, 1);
    save_btns_layout->setContentsMargins(0, 0, 0, 0);
    QWidget* save_btns = new QWidget();
    save_btns->setLayout(save_btns_layout);

    // Open the control_states configuration file and create a hash map of the 
    QFile control_state_file(this->root->get_configuration()->filePath("control_states.cfg"));
    if (control_state_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&control_state_file);

        while (!in.atEnd()) {
            // The first line gives name and placement
            // The second line gives valve, ignitor, and personnel safety info
            QStringList l1 = in.readLine().split(',');
            QStringList l2 = in.readLine().split(',');

            // Splits the name, row, and col sublists from the first line
            QStringList names = l1[0].split('/');
            QStringList rows = l1[1].split('/');
            QStringList cols = l1[2].split('/');

            // Create a hashmap for the control state
            QHash<QString, int>* cur_state = new QHash<QString, int>;

            // Loop through the listed valves and add them as key:value pairs to the control state
            for (int i = 0; i < l2.size(); ++i) {
                QStringList valve = l2[i].split(':');
                cur_state->insert(valve[0], valve[1].toInt());
            }

            // Add this control state (with the combined name) to the hash of all control states
            this->control_states.insert(l1[0], cur_state);

            // Create a button for each name, add it to the specified spot, and connect it to the proper function
            for (int i = 0; i < names.size(); ++i) {
                QPushButton* btn = new QPushButton(names[i]);
                operation_layout->addWidget(btn, rows[i].toInt(), cols[i].toInt());
            }
        }
    }

    // Create label for the control state
    new Standard_Label("Control State:", valve_layout, 100);
    this->control_state->setAlignment(Qt::AlignCenter);
    valve_layout->addWidget(this->control_state, 100, 1);
    this->update_control_state(); // Properly initialize its text

    // Create dummy widget for valve layout and operation layout
    QWidget* valve_widget = new QWidget;
    valve_widget->setLayout(valve_layout);
    QWidget* operation_widget = new QWidget;
    operation_widget->setLayout(operation_layout);

    // Create tab widget, attach pages
    QTabWidget* tab = new QTabWidget;
    tab->addTab(valve_widget, "Valves");
    tab->addTab(operation_widget, "Operations");

    // Create a dummy layout to hold the tab widget and save buttons
    QGridLayout* bottom_layout = new QGridLayout;
    bottom_layout->addWidget(tab, 0, 0, -1, 1, Qt::AlignLeft | Qt::AlignTop);
    bottom_layout->addWidget(save_btns, 0, 1, Qt::AlignRight | Qt::AlignTop);
    bottom_layout->setContentsMargins(5, 5, 5, 5);
    QFrame* bottom_widget = new QFrame;
    bottom_widget->setLayout(bottom_layout);
    Frame_with_Title* layout = new Frame_with_Title("Control Information", bottom_widget);

    this->setLayout(layout);
    this->root->add_to_main_window(this, 2, 0);
}

void GUI_CTRL_Window::update_control_state() {
    // Loop through all control states until the first one which is satisfied
    QStringList states = this->control_states.keys();
    for (int i = 0; i < states.size(); ++i) {
        QString name = states[i];
        QHash<QString, int>* cur_state = this->control_states[name];

        // Loop through all valves and check if they match the current control state
        bool is_control_state = true;
        for (int j = 0; j < this->valves->size(); ++j) {
            Valve* valve = this->valves->at(j);
            QString ID = valve->get_ID();

            // If the control state does not mention the current valve, it is considered to pass
            if (!cur_state->contains(ID)) {
                continue;
            }

            // Check if the current valve has the same state
            if (cur_state->value(ID) != valve->get_state()) {
                is_control_state = false;
                break;
            }
        }

        // Consider checking if any valve is mentioned in the control state, but not in the system
        // Should not happen if control_states.cfg is setup properly...

        // If the control state is correct, update the display and return
        if (is_control_state) {
            this->control_state->setText(name);
            return;
        }
    }

    // If no control state matches, update the display and print to the console
    this->control_state->setText("Unknown State");
    cout << "No control state matches" << endl;
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
    // Since save activates every time a valve is updated, this can pass through the signal
    // without needing another connection
    this->update_control_state();

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
    // outputs current control state (just updated) and last command
    out << ',' << this->control_state->text() << ',' << command << endl;
}

void GUI_CTRL_Window::sm_new_state(QString new_state) {
    // TODO: Add check if SM is enabled
    // TODO: Implement
    cout << new_state.toStdString() << endl;
}

void GUI_CTRL_Window::sm_allowed_states(QStringList* allowed_states) {
    // TODO: Add check if SM is enabled
    // TODO: Implement
    cout << allowed_states->at(0).toStdString() << endl;
}
