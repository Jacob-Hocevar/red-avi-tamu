#include "Sensor.h"
#include "Standard_Label.h"
#include <QGridLayout>
#include <Qt>

// Constructors
Sensor::Sensor() : QWidget(), ID(""), name(""), unit(""), data_label(nullptr) {}

Sensor::Sensor(QString ID, QString name, QString unit) :
    QWidget(), ID(ID), name(name), unit(unit), data_label(new QLabel("-")) {
    // Construct a layout to hold the labels
    QGridLayout* layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    this->setLayout(layout);

    // Create label for sensor
    this->create_label();

    // Create output for the sensor
    this->data_label->setAlignment(Qt::AlignRight);
    layout->addWidget(this->data_label, 0, 1);
    }

// Getters
QString Sensor::get_ID() {
    return ID;
}

QString Sensor::get_name() {
    return name;
}

QString Sensor::get_data() {
    return this->data_label->text();
}

// Setters
void Sensor::set_ID(QString ID) {
    this->ID = ID;
    this->create_label();
}

void Sensor::set_name(QString name) {
    this->name = name;
    this->create_label();
}

// Private Helper
void Sensor::create_label() {
    // Get the layout of this widget as a local variable
    // the layout() command returns a generic QLayout*, but it will always be a QGridLayout*
    QGridLayout* layout = static_cast<QGridLayout*>(this->layout());

    QString label = name;
    label.append(" (");
    label.append(ID);
    if ("" != unit) {
        label.append(") [");
        label.append(unit);
        label.append("]:");
    } else {
        label.append(" ):");
    }
    new Standard_Label(label, layout, 0);
}

void Sensor::update_data(const QString& data) {
    this->data_label->setText(data);
}
