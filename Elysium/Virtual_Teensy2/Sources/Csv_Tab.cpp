#include "Csv_Tab.h"

#include "CSVProcessor.h"
#include "Globals.h"

#include <QString>
#include <QLabel>
#include <QPalette>
#include <QDir>
#include <QDebug>
#include <QStringList>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSpacerItem>
#include <QTextStream>

#include <iostream>
using std::cout;
using std::endl;

// const QString PORT = "/dev/Virtual_Teensy";
// const int BAUD = 115200;

Csv_Tab::Csv_Tab(QWidget *parent) : QWidget(parent) {

    setWindowTitle("Configure Test Run");
    // Layout Setup
    QVBoxLayout *mainLayout = new QVBoxLayout(this);  // Main vertical layout
    QFormLayout *formLayout = new QFormLayout();      // Form style layout for input fields
    QHBoxLayout *buttonLayout = new QHBoxLayout();    // Horizontal layout for buttons



    csv_options = new QComboBox(this);
    delay = new QComboBox(this);
    run = new QPushButton("Run", this);

    csv_options->setMaximumWidth(200);
    delay->setMaximumWidth(100);
    run->setMaximumWidth(100);

    formLayout->addRow("Test Data:", csv_options);
    formLayout->addRow("Delay (ms):", delay);


    buttonLayout->addWidget(run);

    // ad to the main layout
    mainLayout->addStretch();
    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();

    // Set layout
    setLayout(mainLayout);

    //this->layout->addWidget(exit, 2, 0);
    this->CSV = nullptr;
    //csv_options
    QString dirPath = "Data";
    QDir dir(dirPath);
    if (!dir.exists()) {
        qWarning() << "Directory" << dirPath << "does not exist.";
        return;
    }else{
        QStringList files = dir.entryList(QStringList() << "*.csv", QDir::Files);
        //qDebug() << files;
        for(const QString& file : files){
            this->csv_options->addItem(file);
        }
    
    }
    // Delay options
    QStringList delay_options = {"50", "100", "150", "200", "300", "400"};
    for(const QString& option : delay_options){
        this->delay->addItem(option);
    }
    //run config
    QObject::connect(this->run, &QPushButton::clicked, this, &Csv_Tab::run_slot);
    QObject::connect(shared_ser, SIGNAL(readyRead()), this, SLOT(read_data())); // added frin vt.cpp


    
}
void Csv_Tab::read_data(){
    QTextStream in(shared_ser->readAll());
    bool first = true;
    while (!in.atEnd()) {
        QString text = in.readLine();
        if ("nop" == text) {
            continue;
        }
        if (first) {
            cout << "Read:";
            first = false;
        }

        cout << '\t' << text.toStdString() << "\r\n";
    }
    if (!first) {
        cout << endl;
    }   
}
void Csv_Tab::run_slot(){
    if (this->CSV) {
        delete this->CSV;
    }

    QString selected_csv = + "Data/" + this->csv_options->currentText();
    int selected_delay = this->delay->currentText().toInt();
    qDebug()<< "running with: "<< selected_csv <<" Baud Rate: "<< selected_delay;
    this->CSV = new CSVProcessor(selected_csv, selected_delay, this);
    QObject::connect(this->CSV, &CSVProcessor::new_pressure, this, &Csv_Tab::write_data);
    QObject::connect(this, &Csv_Tab::stop_running, this->CSV, &CSVProcessor::stop);
    QObject::connect(this, &Csv_Tab::start_running, this->CSV, &CSVProcessor::start);

    this->CSV->readCSV();

}


void Csv_Tab::write_data(QString line){
    qDebug() << line;
    line += "\r\n";
    shared_ser->write(line.toUtf8());
}

void Csv_Tab::pause() {
    emit stop_running();
    if (shared_ser->isOpen()) {
        shared_ser->close();
        qDebug() << "csv closed";


        // add method to stop the csv reading 
    }
}

void Csv_Tab::start() {
    emit start_running();
    if (!shared_ser->isOpen()) {
        shared_ser->setPortName(PORT);
        shared_ser->setBaudRate(BAUD);
        shared_ser->open(QSerialPort::ReadWrite);
        qDebug() << "csv connected";
    }
}