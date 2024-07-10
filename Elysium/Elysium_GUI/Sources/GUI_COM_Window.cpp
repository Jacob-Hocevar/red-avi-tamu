#include "GUI_COM_Window.h"
#include "GUI_Main_Window.h"
#include "Standard_Label.h"
#include <QMessageBox>
#include <QGridLayout>
#include <QObject>
#include <QFrame>
#include <Qt>

// For testing only
#include<iostream>
using std::cout;
using std::endl;

/*
    Initializes the Communications Window for the GUI.
*/
GUI_COM_Window::GUI_COM_Window(GUI_Main_Window* parent):
    QWidget(), root(parent), current_port_name(""), current_baud_rate(""),
    enable_disconnect_confirmation(true), is_connected(false), COM_selection(new QComboBox()),
    baud_selection(new QComboBox()), refresh(new QPushButton), connect(new QPushButton()) {
    
    // Layout for the buttons and labels
    QGridLayout* bottom_layout = new QGridLayout();
    bottom_layout->setContentsMargins(5, 5, 5, 5);
    bottom_layout->setSpacing(10);

    // COM Port and Baud Rate selector boxes and labels
    new Standard_Label("COM Port:", bottom_layout, 0);
    new Standard_Label("Baud Rate:", bottom_layout, 1);

    QObject::connect(COM_selection, SIGNAL(currentTextChanged(const QString&)),
            this, SLOT(update_port(const QString&)));
    QObject::connect(baud_selection, SIGNAL(currentTextChanged(const QString&)),
            this, SLOT(update_baud(const QString&)));

    //this->baud_selection.setMaxVisibleItems(16);
    bottom_layout->addWidget(this->COM_selection, 0, 1);
    bottom_layout->addWidget(this->baud_selection, 1, 1);


    // Construct refresh button to check and update both lists (also resets selected item to default)
    this->refresh->setText("Refresh");
    QObject::connect(refresh, SIGNAL(clicked()),
            this, SLOT(update_COM_options()));
    QObject::connect(refresh, SIGNAL(clicked()),
            this, SLOT(update_baud_options()));
    bottom_layout->addWidget(this->refresh, 0, 2);

    // Connect button attempts to open the serial connection
    this->connect->setText("Connect");
    QObject::connect(connect, SIGNAL(clicked()),
            this, SLOT(connect_to_serial()));
    bottom_layout->addWidget(this->connect, 1, 2);

    // Initialize options for the selection menus [must be done after they are defined, to set states]
    this->update_COM_options();
    this->update_baud_options();

    QFrame* bottom_widget = new QFrame();
    bottom_widget->setLayout(bottom_layout);
    Frame_with_Title* layout = new Frame_with_Title("  Teensy Connection", bottom_widget);

    this->setLayout(layout);
    this->setFixedHeight(106);
    this->root->add_to_main_window(this, 1, 0);
}

/*
    Public functions
*/

bool GUI_COM_Window::get_is_connected() {
    return is_connected;
}

/*
    Private functions
*/

void GUI_COM_Window::update_COM_options() {

}

void GUI_COM_Window::update_port(const QString& text) {
    // Nonsense operations to avoid 'unused parameter' compiler warning
    bool tmp = text.isEmpty();
    tmp = !tmp;
}

void GUI_COM_Window::update_baud_options() {

}

void GUI_COM_Window::update_baud(const QString& text) {
    // Nonsense operations to avoid 'unused parameter' compiler warning
    bool tmp = text.isEmpty();
    tmp = !tmp;
}

void GUI_COM_Window::check_connection_button() {

}

void GUI_COM_Window::connect_to_serial() {

}

void GUI_COM_Window::serial_open() {

}

void GUI_COM_Window::serial_close() {

}
