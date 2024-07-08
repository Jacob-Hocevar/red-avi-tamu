#include "GUI_COM_Window.h"
#include <QMessageBox>
#include <Qt>

// For testing only
#include<iostream>
using std::cout;
using std::endl;

/*
    Initializes the Main Window for the GUI.
*/
GUI_COM_Window::GUI_COM_Window() {
}

GUI_COM_Window::~GUI_COM_Window() {

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

void GUI_COM_Window::update_port(std::string) {

}

void GUI_COM_Window::update_baud_options() {

}

void GUI_COM_Window::update_baud(std::string) {

}

void GUI_COM_Window::check_connection_button() {

}

void GUI_COM_Window::connect_to_serial() {

}

void GUI_COM_Window::serial_open() {

}

void GUI_COM_Window::serial_close() {

}
