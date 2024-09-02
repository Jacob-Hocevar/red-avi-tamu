#include "Virtual_Teensy.h"
#include "VT_Sensor.h"
#include <QRandomGenerator>
#include <QGridLayout>
#include <QTextStream>
#include <QPushButton>
#include <Qt>

#include <QDebug>
#include <iostream>
using std::cout;
using std::endl;

const QString PORT = "/dev/Virtual_Teensy";
const int BAUD = 115200;

Virtual_Teensy::Virtual_Teensy(int interval, QFile* config):
    QWidget(), interval(interval), config(config), interval_label(new QLabel()),
    interval_slider(new QSlider()), ser(new QSerialPort()), timer(new QTimer()) {
    // Layout to attach internal widgets to
    QGridLayout* layout = new QGridLayout();
    layout->setSpacing(10);
    layout->setContentsMargins(5,5,5,5);

    // Label for interval selection slider
    QLabel* interval_title = new QLabel("Time between sending data:");
    interval_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(interval_title, 0, 0);
    this->interval_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(this->interval_label, 0, 1);

    // Configure Slider for the interval and add to layout
    this->interval_slider->setMinimum(1);
    this->interval_slider->setMaximum(500);
    this->interval_slider->setValue(this->interval);
    this->interval_slider->setOrientation(Qt::Horizontal);
    QObject::connect(interval_slider, SIGNAL(valueChanged(int)), this, SLOT(set_interval(int)));
    this->set_interval(this->interval);
    layout->addWidget(this->interval_slider, 1, 0, 1, 2);

    // Button to add new virtual sensors
    QPushButton* add_sensor_btn = new QPushButton("Add new virtual sensor");
    QObject::connect(add_sensor_btn, SIGNAL(clicked()), this, SLOT(add_sensor()));
    layout->addWidget(add_sensor_btn, 2, 0, 1, 2);

    // TODO: Seperator between top and set of sensors

    // TODO: Add all sensors from the config file
    VT_Sensor* test_empty = new VT_Sensor();
    layout->addWidget(test_empty, 3, 0, 1, 2);
    VT_Sensor* test_configured = new VT_Sensor("P3", QString::number(14), QString::number(800),
        QString::number(0), QString::number(1.1), true);
    layout->addWidget(test_configured, 4, 0, 1, 2);

    // Attach the layout to this object
    this->setLayout(layout);

    /*
        Non UI Elements
    */

    // Setup and connect to the GUI
    this->ser->setPortName(PORT);
    this->ser->setBaudRate(BAUD);
    this->ser->open(QSerialPort::ReadWrite);

    // If connection fails, print debug info
    if (!this->ser->isOpen()) {
        qDebug() << this->ser->error();
    }

    // Reads are detected automatically, writes every this->interval
    QObject::connect(this->ser, SIGNAL(readyRead()), this, SLOT(read_data()));
    QObject::connect(this->timer, SIGNAL(timeout()), this, SLOT(write_data()));
    this->timer->start(this->interval);
}

Virtual_Teensy::~Virtual_Teensy() {
    // Close and delete non-UI elements
    this->ser->close();
    delete this->ser;
    delete this->timer;

    // Save the current configuration
}

void Virtual_Teensy::set_interval(int interval) {
    this->interval = interval;
    this->interval_label->setText(QString::number(interval) + " ms");
    this->timer->setInterval(interval);
}

void Virtual_Teensy::read_data() {
    cout << "Read: ";
    QTextStream in(this->ser->readAll());
    while (!in.atEnd()) {
        cout << in.readLine().toStdString();
    }
    cout << endl;
}

void Virtual_Teensy::write_data() {
    QString data = "P1:";
    data.append(QString::number(9 + 2*QRandomGenerator::global()->generateDouble()));
    data.append(",P2:");
    data.append(QString::number(28 + 4*QRandomGenerator::global()->generateDouble()));
    
    // Uncomment if you need to confirm the output, otherwise it spams the terminal
    // cout << "Write:\t" << data.toStdString() << "\\r\\n" << endl;

    data.append("\r\n");
    this->ser->write(data.toUtf8()); 
}

void Virtual_Teensy::add_sensor() {

}
