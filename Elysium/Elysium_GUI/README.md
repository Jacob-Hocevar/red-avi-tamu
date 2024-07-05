# How to start the GUI

First navigate so that your active directory is `Elysium_GUI`.

If you have made no changes/it is already compiled, simply run `./Elysium_GUI`.

If you have just changed a couple files, but not added significant includes or new files:
1. `make` to compile.
2. `./Elysium_GUI` to run.


If you have made signficant changes (new files or substantive includes):
1. `qmake -project` to (re)create the project file.
2. Update the project file:

    A. Set `INCLUDEPATH = ./Headers` so it can locate the header files.

    To let the makefile/compiler know where to find core includes:

    B. Add a line reading `QT += widgets` for QApplication and QMainWindow, among others.

    C. Add a line reading `QT += gui` for QPixmap.

    D. Add a line reading `QT += core` for the namespace (alignment flags, among others).

    [comment]: # (Update Here if new lines become necessary)

3. `qmake Elysium_GUI.pro` to generate a makefile from the project file.
4. `make` to compile (this is where errors will show up with improper code).
5. `./Elysium_GUI` to run.

## Setup
Use a unix-like enviornment and install the proper libraries.

In my case, the command is `sudo apt install make` to install `make`.

You will need the `g++` compiler and the libraries `make`, `qmake`, and `qtdeclarative5-dev`.
