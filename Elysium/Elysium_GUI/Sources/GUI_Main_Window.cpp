#include "GUI_Main_Window.h"
#include "GUI_COM_Window.h"
#include <QWidget>
#include <QPixmap>
#include <QPalette>
#include <QMessageBox>
#include <Qt>

// For testing only
#include<iostream>
using std::cout;
using std::endl;

/*
    Initializes the Main Window for the GUI.
*/
GUI_Main_Window::GUI_Main_Window():
    QMainWindow(), enable_shutdown_confirmation(true),
    logo(new QLabel()), layout(new QGridLayout()), com_menu(nullptr) {
        // Sets the initial size of the GUI
        this->manual_resize(320, 150);

        this->layout->setContentsMargins(2, 2, 2, 2);
        this->layout->setSpacing(10);

        // TODO: Two versions of the logo for light/dark mode.
        // RED Logo:
        this->logo->setPixmap(QPixmap("Assets/RED_logo.png"));
        this->logo->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        this->logo->setMaximumHeight(30);
        this->set_color(logo, Qt::red);
        this->layout->addWidget(this->logo, 0, 0);

        cout << "Main 1" << endl;
        // Create the COM Window
        //GUI_COM_Window* test = new GUI_COM_Window(this);
        this->com_menu = new GUI_COM_Window(this);
        cout << "Main 2" << endl;

        //this->layout->addWidget(this->com_menu, 1, 0);

        //this->com_menu = test;

        // Creates a dummy widget to hold the layout and attach to the window
        QWidget* w = new QWidget();
        w->setLayout(layout);
        this->setCentralWidget(w);
        cout << "Main 3" << endl;
}


/*
    Public functions
*/

void GUI_Main_Window::add_to_main_window(QWidget* widget, int row, int col, int row_span, int col_span) {
    this->layout->addWidget(widget, row, col, row_span, col_span);
}

void GUI_Main_Window::remove_from_main_window(QWidget* widget) {
    this->layout->removeWidget(widget);
}

void GUI_Main_Window::set_color(QWidget* widget, const QColor& color) {
    QPalette palette = QPalette();
    palette.setColor(QPalette::Window, color);

    widget->setAutoFillBackground(true);
    widget->setPalette(palette);
}

/*
    Private functions
*/

void GUI_Main_Window::manual_resize(int width, int height) {
    this->setFixedSize(width, height);
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    this->setMinimumSize(0, 0);
}

void GUI_Main_Window::closeEvent(QCloseEvent* event) {
    // If you are not connected to the Teensy, no confirmation needed
    if (!com_menu || !this->com_menu->get_is_connected()) {
        delete com_menu;
        event->accept();
        return;
    }

    QMessageBox::StandardButton confirmation = QMessageBox::Apply;
    if (this->enable_shutdown_confirmation) {
        confirmation = QMessageBox::warning(this, "Confirmation", "Are you sure you want to exit the GUI?",
                        QMessageBox::Cancel | QMessageBox::Apply);
    }

    if (QMessageBox::Apply == confirmation) {
        delete com_menu;
        event->accept();
    } else {
        event->ignore();
    }
}