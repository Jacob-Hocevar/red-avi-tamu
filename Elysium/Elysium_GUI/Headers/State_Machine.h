#ifndef State_Machine_H
#define State_Machine_H

#include <QStringList>
#include <QString>
#include <QHash>
#include <QFile>

// Constant Declarations, not definitions to avoid multiply defined objects with all files that include this header
// Defined in the .cpp file
extern const int SEQUENCE_DELAY;
extern const int PURGE_DELAY;
extern const int IGNITION_DELAY;
extern const int PURGE_DURATION;
extern const int FIRE_DURATION;
extern const int APG_ABORT_DURATION;

// TODO: Consider using derived classes for each configuration. Make update_signals() a pure virtual function.
// This class must derive from QObject to use signals/slots
class State_Machine : public QObject {
    Q_OBJECT
public:
    // Constructor
    State_Machine(QString name, QHash<QString, double>* data);
    void start();
private:
    QString config_name;
    QString cur_state;
    bool people_safe_dist;
    QStringList* cur_allowed_states;

    // TODO: Determine best way to get access to relevant data.
    // One option is to modify the GUI_DAQ to have hash map on the heap,
    // then share the pointer here.
    // Another option is to make a hash map here, and use a slot/signal
    // to update only the relevant data, though it is still stored twice.
    QHash<QString, double>* cur_data;

    // List of timestamps for autoaborts
    QList<int>* apg_times;

    void hotfire_1(bool new_state, bool abort);
private slots:
    void set_state(QString state);
    void set_people_safe_dist(int safe);
    void new_data();
    void update_signals(bool new_state, bool abort=false);

signals:
    void new_state(QString state);
    void allowed_states(QStringList* allowed_states);
};

#endif