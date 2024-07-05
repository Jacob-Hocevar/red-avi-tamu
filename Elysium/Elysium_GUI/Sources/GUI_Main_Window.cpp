#include "GUI_Main_Window.h"
#include <QWidget>
#include <QPixmap>
#include <Qt>

// For dynamic filepath
// #include <unistd.h>

// For testing only
#include<iostream>
using std::cout;
using std::endl;

/*
    Initializes the Main Window for the GUI.
*/
GUI_Main_Window::GUI_Main_Window():
    QMainWindow(), enable_shutdown_confirmation(false),
    logo(), layout() {
        // Sets the initial size of the GUI
        this->manual_resize(320, 150);

        this->layout.setContentsMargins(2, 2, 2, 2);
        this->layout.setSpacing(10);

        // TODO: Two versions of the logo for light/dark mode.
        // RED Logo:

        // Method to get path to current working directory, need to append to it.
        /*
        const size_t size = 1024;
        char buffer[size];
        if (getcwd(buffer, size) != NULL) {
            cout << "Current Working Directory: " << buffer << endl;
        } else {
            cout << "Error getting current working directory" << endl;
        }

        buffer += "/Assets/RED_logo.png";
        cout << buffer << endl;
        */

        // Try local path only, since user must be in the correct CWD:
        this->logo.setPixmap(QPixmap("Assets/RED_logo.png"));
        this->logo.setAlignment(Qt::AlignLeft | Qt::AlignTop);
        this->logo.setMaximumHeight(30);
        this->layout.addWidget(&this->logo, 0, 0);


        // Creates a dummy widget to hold the layout and attach to the window
        QWidget* w = new QWidget();
        w->setLayout(&layout);
        this->setCentralWidget(w);
}


/*
    Public function
*/
void GUI_Main_Window::add_to_main_window(QWidget* widget, int row, int col, int row_span=1, int col_span=1) {
    this->layout.addWidget(widget, row, col, row_span, col_span);
}

void GUI_Main_Window::remove_from_main_window(QWidget* widget) {
    this->layout.removeWidget(widget);
}

void GUI_Main_Window::manual_resize(int width, int height) {
    this->setFixedSize(width, height);
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    this->setMinimumSize(0, 0);
}
