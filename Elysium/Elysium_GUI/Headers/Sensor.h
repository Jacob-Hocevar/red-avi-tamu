#ifndef Sensor_H
#define Sensor_H

#include <QSerialPort>
#include <QString>

class Sensor {
public:
    // Constructors 
    Sensor();
    Sensor(QString ID, QString name);

    // Getter/Setter functions
    QString get_ID();
    QString get_name();
    void set_ID(QString ID);
    void set_name(QString name);
private:
    QString ID;
    QString name;
};

#endif