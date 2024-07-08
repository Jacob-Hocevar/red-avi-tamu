#ifndef GUI_Main_Window_H
#define GUI_Main_Window_H

// #include "GUI_COM_Window.h"
#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QColor>
#include <QCloseEvent>

/*
    The Main Window for the GUI.
    Contains the layout which all major elements are attached to.
*/
class GUI_Main_Window : public QMainWindow {
public:
    // Inherits the constructors (default, paramterized, copy, and move) from the base class
    // The default will be overwritten in the source file
    // using QMainWindow::QMainWindow;
    GUI_Main_Window();
    void add_to_main_window(QWidget* widget, int row, int col, int row_span, int col_span);
    void remove_from_main_window(QWidget* widget);
    void set_color(QWidget* widget, const QColor& color);

private:
    bool enable_shutdown_confirmation;
    QLabel logo;
    QGridLayout layout;
    // GUI_COM_Window com_menu;

    void manual_resize(int width, int height);
    void closeEvent(QCloseEvent* event);
};


#endif