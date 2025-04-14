#include "Globals.h"

QSerialPort* shared_ser = new QSerialPort();
const QString PORT = "/dev/Virtual_Teensy";
const int BAUD = 115200;