#ifndef GUI_APP_H
#define GUI_APP_H

#include <QApplication>
// This class is just a copy subclass of QApplication to reduce includes in main.cpp

/*
    Generates main instance of application.
    Accepts Command Line arguments.
*/
class GUI_APP : public QApplication {
    // Inherits the constructors (default, paramterized, copy, and move) from the base class
    using QApplication::QApplication;
};

#endif