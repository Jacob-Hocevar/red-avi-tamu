#include "Virtual_Teensy.h"
#include <QCoreApplication>
#include <QObject>

#include <QDebug>
#include <iostream>
using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Virtual_Teensy vt;

    return app.exec();
}
