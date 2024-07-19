#include "Sensor.h"

// Constructors
Sensor::Sensor() : ID(""), name("") {}

Sensor::Sensor(QString ID, QString name) :
    ID(ID), name(name) {}

// Getters
QString Sensor::get_ID() {
    return ID;
}

QString Sensor::get_name() {
    return name;
}

// Setters
void Sensor::set_ID(QString ID) {
    this->ID = ID;
}

void Sensor::set_name(QString name) {
    this->name = name;
}
