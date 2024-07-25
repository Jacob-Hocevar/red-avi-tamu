#ifndef GUI_DAQ_Window_H
#define GUI_DAQ_Window_H

#include "GUI_Main_Window.h"
#include "Sensor.h"
#include <QSerialPort>
#include <QStringList>

// TODO: implement
class GUI_DAQ_Window : public QWidget {
    Q_OBJECT
public:
    // Inherits the constructors (default, paramterized, copy, and move) from the base class
    // The parameterized constructor and destructor will be overwritten in the source file
    using QWidget::QWidget;
    GUI_DAQ_Window(GUI_Main_Window* parent, QSerialPort* ser);
private:
    GUI_Main_Window* root;
    QSerialPort* ser;

    QHash<QString, Sensor*> sensors;
    QStringList derived_IDs;

    void update_derived(const QString& ID);
private slots:
    void update_sensors();
};


#endif