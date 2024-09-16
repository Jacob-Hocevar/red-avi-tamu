#ifndef State_Machine_H
#define State_Machine_H

#include <QString>
#include <QFile>

class State_Machine {
public:
    // Constructor
    State_Machine(const QFile& configuration);
private:
    QFile configuration;
    QString config_name;
    QString cur_state;

    // TODO: Determine best way to get access to relevant data.
    // One option is to modify the GUI_DAQ to have hash map on the heap,
    // then share the pointer here.
    // Another option is to make a hash map here, and use a slot/signal
    // to update only the relevant data, though it is still stored twice.
    QHash<QString, double>* cur_data;

private slots:
    void set_state(QString state);

signals:
    void new_state(QString state);
    void allowed_states(QStringList allowed_states);
    void prohibited_states(QStringList allowed_states);
};

#endif