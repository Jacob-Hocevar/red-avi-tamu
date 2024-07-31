#ifndef GUI_CTRL_Window_H
#define GUI_CTRL_Window_H

#include "GUI_Main_Window.h"
#include "Valves.h"
#include <QSerialPort>

// TODO: implement
class GUI_CTRL_Window : public QWidget {
    Q_OBJECT
public:
    // Inherits the constructors (default, paramterized, copy, and move) from the base class
    // The parameterized constructor and destructor will be overwritten in the source file
    using QWidget::QWidget;
    GUI_CTRL_Window(GUI_Main_Window* parent, QSerialPort* ser);

private:
    GUI_Main_Window* root;
    QSerialPort* ser;

    QList<Valve*>* valves;
    bool is_saving;
    QFile* data_file;
    QPushButton* start_save_btn;
    QPushButton* end_save_btn;

private slots:
    void start_save();
    void end_save();
};


#endif