#include "CSVProcessor.h"

#include <QTime>
#include <QMap>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QEventLoop>


CSVProcessor::CSVProcessor(const QString& filePath, int delay_ms, QWidget *parent)
    :QWidget(parent), filePath(filePath), delay_ms(delay_ms){


    

} 



void CSVProcessor::readCSV(){
    QFile file(filePath);
    // If not csv found pop up box
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox msg;
        msg.setText("No csv file found");
        msg.setIcon(QMessageBox::Warning);
        msg.setWindowTitle("ERROR");
        msg.exec();
        return;
    }
    running = true;
    //  read line by line csv
    QTextStream in(&file);
    while (!in.atEnd() && running) {
        QString line = in.readLine();
  
        emit new_pressure(line);
        delay(delay_ms);

    }
}


    

void CSVProcessor::delay(int millisecondsToWait ){
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void CSVProcessor::stop(){
    running = false;
}
void CSVProcessor::start(){
    running = true;
}
