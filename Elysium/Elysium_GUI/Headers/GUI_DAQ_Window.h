#ifndef GUI_DAQ_Window_H
#define GUI_DAQ_Window_H

#include "GUI_Main_Window.h"
#include <QSerialPort>

// TODO: implement
class GUI_DAQ_Window : public QWidget {
    Q_OBJECT
public:
    // Inherits the constructors (default, paramterized, copy, and move) from the base class
    // The parameterized constructor and destructor will be overwritten in the source file
    using QWidget::QWidget;
    GUI_DAQ_Window(GUI_Main_Window* parent, QSerialPort* ser);
    void start();

private:
    GUI_Main_Window* root;
    QSerialPort* ser;

private slots:
};


#endif