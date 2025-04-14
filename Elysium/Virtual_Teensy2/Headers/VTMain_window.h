#ifndef VTMAIN_WINDOW_H
#define VTMAIN_WINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QTabWidget>
#include  <QSerialPort>
#include <QFile>

class VTMain_window : public QMainWindow{
    Q_OBJECT

public:
    VTMain_window();

private:
    QTabWidget *tab_widget;
    QGridLayout *layout;
    QFile *config;
    //QSerialPort *shared_ser;
private slots:
    void onTabChange(int tab);
    //void load_config(QTabWidget *tab_widget);
};

#endif