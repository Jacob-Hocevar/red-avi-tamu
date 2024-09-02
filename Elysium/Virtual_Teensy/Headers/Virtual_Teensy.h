#ifndef Virtual_Teensy_H
#define Virtual_Teensy_H

#include <QSerialPort>
#include <QWidget>
#include <QString>
#include <QSlider>
#include <QTimer>
#include <QLabel>
#include <QFile>

#include <QDebug>
#include <iostream>
using std::cout;
using std::endl;

extern const QString PORT;
extern const int BAUD;

class Virtual_Teensy : public QWidget {
    Q_OBJECT
public:
    using QWidget::QWidget;
    Virtual_Teensy(int interval, QFile* config);
    ~Virtual_Teensy();
private:
    int interval;
    QFile* config;

    QLabel* interval_label;
    QSlider* interval_slider;
    QSerialPort* ser;
    QTimer* timer;
private slots:
    void set_interval(int interval);
    void read_data();
    void write_data();
    void add_sensor();
};

#endif