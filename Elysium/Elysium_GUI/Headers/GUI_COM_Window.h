#ifndef GUI_COM_Window_H
#define GUI_COM_Window_H

#include "Frame_with_Title.h"
#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QGridLayout>
#include <QString>

// Forward declaration of mainwindow to avoid circular dependencies
class GUI_Main_Window;

/*
    The Main Window for the GUI.
    Contains the layout which all major elements are attached to.
*/
class GUI_COM_Window : public QWidget {
    Q_OBJECT
public:
    
    // Inherits the constructors (default, paramterized, copy, and move) from the base class
    // The parameterized constructor and destructor will be overwritten in the source file
    using QWidget::QWidget;
    GUI_COM_Window(GUI_Main_Window* parent);
    ~GUI_COM_Window();
    bool get_is_connected();

private:
    GUI_Main_Window* root;
    QString current_port_name;
    QString current_baud_rate;

    bool enable_disconnect_confirmation;
    bool is_connected;
    QComboBox* COM_selection;
    QComboBox* baud_selection;
    QPushButton* refresh;
    QPushButton* connect;

    // Temp
    Frame_with_Title* layout;

    // teensy_ports (list of port info) [May be able to only use this as a locally scoped variable]
    // ser (serial connection type)
    // CTRL (GUI_CTRL_Window) [Neither of these should reference GUI_COM_Window, so no forward declarations]
    // DAQ (GUI_DAQ_Window) [Neither of these should reference GUI_COM_Window, so no forward declarations]

    void check_connection_button();
    void serial_open();
    void serial_close();

private slots:
    void update_COM_options();
    void update_baud_options();
    void update_port(const QString& text);
    void update_baud(const QString& text);
    void connect_to_serial();
};


#endif