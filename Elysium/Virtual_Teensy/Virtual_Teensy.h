#include <QSerialPortInfo>
#include <QSerialPort>
#include <QObject>
#include <QTimer>
#include <QString>

#include <QDebug>
#include <iostream>
using std::cout;
using std::endl;

const QString PORT = "/dev/Virtual_Teensy";
const int BAUD = 115200;

const int INTERVAL = 10;   // Time in ms between writes to serial signal.

class Virtual_Teensy : public QObject {
    Q_OBJECT
public:
    Virtual_Teensy();
    ~Virtual_Teensy();
private:
    QSerialPort* ser;
    QTimer* timer;
    QByteArray last_read;
private slots:
    void read_data();
    void write_data();
};
