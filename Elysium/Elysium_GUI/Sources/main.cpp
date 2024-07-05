#include "GUI_APP.h"
#include "GUI_Main_Window.h"

int main (int argc, char *argv[]) {
    // Creates an instance of QApplication
    GUI_APP gui_app(argc, argv);

    // This is our MainWindow class containing our GUI and functionality
    GUI_Main_Window gui_main_window;
    gui_main_window.show();

    // Run the application and return execs() return value/code
    return gui_app.exec();
}
