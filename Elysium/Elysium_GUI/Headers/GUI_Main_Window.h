#ifndef GUI_Main_Window_H
#define GUI_Main_Window_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QColor>
#include <QCloseEvent>

// Forward declaration of COM Window to avoid circular dependencies
class GUI_COM_Window;

/*
    The Main Window for the GUI.
    Contains the layout which all major elements are attached to.
*/
class GUI_Main_Window : public QMainWindow {
    Q_OBJECT
public:
    // Inherits the constructors (default, paramterized, copy, and move) from the base class
    // The default will be overwritten in the source file
    using QMainWindow::QMainWindow;
    GUI_Main_Window();
    void add_to_main_window(QWidget* widget, int row, int col, int row_span=1, int col_span=1);
    void remove_from_main_window(QWidget* widget);
    void set_color(QWidget* widget, const QColor& color);
    void manual_resize(int width, int height);

private:
    bool enable_shutdown_confirmation;
    QLabel* logo;
    QGridLayout* layout;
    GUI_COM_Window* com_menu;

    void closeEvent(QCloseEvent* event);
};


#endif