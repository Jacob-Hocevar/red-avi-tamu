#include "State_Machine.h"
#include <QTimer>

// For Testing only
#include <iostream>
using std::cout;
using std::endl;

// Constants (user set parameters)
// Number of milliseconds that the pre-fire and post-fire purges should last
const int PURGE_DURATION = 5000;

// Number of milliseconds that the Fire should last
const int FIRE_DURATION = 3500;

// Number of milliseconds that the ignition system or ball valve signals should be delayed
const int IGNITION_DELAY = 0;

// Underpressure [psi]. If Pressure in the fuel manifold drops below this value immediately shutdown.
// TODO: Confirm value w/ TCA and FSYS.
const int UNDER_PRESSURE = 450;

// Constructor
State_Machine::State_Machine(QString name, QHash<QString, double>* data):
    config_name(name), cur_allowed_states(new QStringList()), cur_data(data) {
    // Set Initial State
    if ("hotfire_1" == this->config_name) {
        this->set_state("Fully Closed");
    } else {
        cout << "State Machine: Unknown configuration: " << this->config_name.toStdString() << endl;
    }

    // If the system autonomously updates the state, it should update the change the same as a manual update
    QObject::connect(this, SIGNAL(new_state(QString)), this, SLOT(set_state(QString)));
}

void State_Machine::hotfire_1(bool new_state) {
    // Determine what states should be allowed, given the current state and safety
    QStringList new_allowed_states = {};

    // Consider each state and failure mode
    if        ("Fully Closed"           == this->cur_state) {
        // The fill state is safe to open
        new_allowed_states << "Filling";

        // Personnel must be at a safe distance before advancing
        if (this->people_safe_dist) {
            new_allowed_states << "Letting GN2 In";
            new_allowed_states << "Shutdown Ph. 2"; // Non-emergency abort
        }

    } else if ("Letting GN2 In"         == this->cur_state) {
        // In this state, it is safe to back to the closed state
        new_allowed_states << "Fully Closed";

        // Personnel must be at a safe distance before advancing
        if (this->people_safe_dist) {
            new_allowed_states << "Hold Pressure";
            new_allowed_states << "Shutdown Ph. 2"; // Non-emergency abort
        }

    } else if ("Hold Pressure"          == this->cur_state) {
        // Personnel must be at a safe distance before changing the state
        if (this->people_safe_dist) {
            new_allowed_states << "Letting GN2 In"; // Return to previous state
            new_allowed_states << "Dry Run";        // Advance
            new_allowed_states << "Shutdown Ph. 2"; // Non-emergency abort
        }
    
    } else if ("Dry Run"                == this->cur_state) {
        // Fully closed/returning to Hold Pressure are not allowed states
        // due to possible fluid hammer when closing LA-BV 1
        // The filling state is identical to the abort state.
        new_allowed_states << "Filling";
    
    } else if ("Filling"                == this->cur_state) {
        // Closing the venting/fill valve is safe
        new_allowed_states << "Fully Closed" << "Pre-Pressurization";
    
    } else if ("Pre-Pressurization"     == this->cur_state) {
        // This state is safe (identical to fully closed)
        // Allowing a name change to fully closed may break the sequencing

        // Personnel must be at a safe distance before changing the state
        if (this->people_safe_dist) {
            new_allowed_states << "Pressurization"; // Advance
            new_allowed_states << "Shutdown Ph. 2"; // Non-emergency abort
        }
    
    } else if ("Pressurization"         == this->cur_state) {
        // Personnel must be at a safe distance before changing the state
        if (this->people_safe_dist) {
            new_allowed_states << "Pre-Pressurization"; // Return to previous state
            new_allowed_states << "Purge";              // Advance
            new_allowed_states << "Shutdown Ph. 2";     // Non-emergency abort
        }
    
    } else if ("Purge"                  == this->cur_state) {
        // Terminal sequence (no returning to previous state)
        // Personnel must be at a safe distance before changing the state
        if (this->people_safe_dist) {
            new_allowed_states << "Purge Shutdown"; // Advance
            new_allowed_states << "Shutdown Ph. 2"; // Non-emergency abort
        }

        // Once this state is reached, after PURGE_DURATION, move to purge shutdown
        if (new_state) {
            QTimer* purge = new QTimer(this);
            purge->setSingleShot(true);
            QObject::connect(purge, SIGNAL(timeout()), this, SLOT([=]() {this->emit new_state("Purge Shutdown");}));
            
            // If the state is changed, the timer should stop to prevent multiple state changes.
            QObject::connect(this, SIGNAL(new_state()), purge, SLOT(stop()));
            purge->start(PURGE_DURATION);
        }

    } else if ("Purge Shutdown"         == this->cur_state) {
        // Terminal sequence (no returning to previous state)
        // Personnel must be at a safe distance before changing the state
        if (this->people_safe_dist) {
            new_allowed_states << "Fire";           // Advance
            // TODO: If Purge Shutdown is modified to open the ball valves, change to Shutdown Ph. 1
            new_allowed_states << "Shutdown Ph. 2"; // Non-emergency abort
        }

        // Once this state is reached, after IGNITION_DELAY, move to fire
        if (new_state) {
            QTimer* ignition = new QTimer(this);
            ignition->setSingleShot(true);
            QObject::connect(ignition, SIGNAL(timeout()), this, SLOT([=]() {this->emit new_state("Fire");}));

            // If the state is changed, the timer should stop to prevent multiple state changes.
            QObject::connect(this, SIGNAL(new_state()), ignition, SLOT(stop()));
            ignition->start(IGNITION_DELAY);
        }
    
    } else if ("Fire"                   == this->cur_state) {
        // Terminal sequence (no returning to previous state)
        // This state change makes the system safer, so while there should be no one near the engine,
        // the button should never be disabled.
        new_allowed_states << "Shutdown Ph. 1"; // Advance (and abort)

        // Once this state is reached, after FIRE_DURATION, move to shutdown ph 1
        if (new_state) {
            QTimer* fire = new QTimer(this);
            fire->setSingleShot(true);
            QObject::connect(fire, SIGNAL(timeout()), this, SLOT([=]() {this->emit new_state("Shutdown Ph. 1");}));

            // If the state is changed, the timer should stop to prevent multiple state changes.
            QObject::connect(this, SIGNAL(new_state()), fire, SLOT(stop()));
            fire->start(FIRE_DURATION);
        }
    
    } else if ("Shutdown Ph. 1"         == this->cur_state) {
        // Shutdown sequence (no returning to previous state)
        if (this->people_safe_dist) {
            new_allowed_states << "Shutdown Ph. 2"; // Advance (continue safing procedure)
        }

        // Once this state is reached, after PURGE_DURATION, move to shutdown ph 2
        if (new_state) {
            QTimer* purge = new QTimer(this);
            purge->setSingleShot(true);
            QObject::connect(purge, SIGNAL(timeout()), this, SLOT([=]() {this->emit new_state("Shutdown Ph. 2");}));

            // If the state is changed, the timer should stop to prevent multiple state changes.
            QObject::connect(this, SIGNAL(new_state()), purge, SLOT(stop()));
            purge->start(PURGE_DURATION);
        }
    
    } else if ("Shutdown Ph. 2"         == this->cur_state) {
        // Shutdown sequence (no returning to previous state)
        if (this->people_safe_dist) {
            new_allowed_states << "Excess Discharge Ph. 1"; // Advance (continue safing procedure)
        }
    
    } else if ("Excess Discharge Ph. 1" == this->cur_state) {
        // Shutdown sequence (no returning to previous state)
        if (this->people_safe_dist) {
            new_allowed_states << "Excess Discharge Ph. 2"; // Advance (continue safing procedure)
        }
    
    } else if ("Excess Discharge Ph. 2" == this->cur_state) {
        // Final shutdown step, can revert to fully closed (power off), if desired
        new_allowed_states << "Fully Closed";
    
    } else {
        cout << "State Machine: Unknown Current State: " << this->cur_state.toStdString() << endl;
    }

    // check if the allowed states are different the last emitted ones
    bool are_equal = new_allowed_states.size() == this->cur_allowed_states->size();
    if (are_equal) {
        for (int i = 0; i < new_allowed_states.size(); ++i) {
            if (new_allowed_states.at(i) != this->cur_allowed_states->at(i)) {
                are_equal = false;
                break;
            }
        }
    }

    // If the states are new, update the variable and emit a new signal
    if (!are_equal) {
        *(this->cur_allowed_states) = new_allowed_states;
        emit allowed_states(this->cur_allowed_states);
    }
}

void State_Machine::set_state(QString state) {
    this->cur_state = state;
    this->update_signals(true);
}

void State_Machine::set_people_safe_dist(bool safe) {
    this->people_safe_dist = safe;
    this->update_signals(false);
}

void State_Machine::new_data() {
    if ("hotfire_1" != this->config_name) {
        cout << "State Machine: Unknown configuration: " << this->config_name.toStdString() << endl;
        return;
    }

    // TODO: Consider Overpressure during Pressurization - Fire. (check PT resolution, FSYS).

    // During the fire state, check if the fuel side has dropped in pressure below the cutoff.
    if ("Fire" == this->cur_state) {
        try {
            double p4 = this->cur_data->value("P4");
            if (p4 <= UNDER_PRESSURE) {
                // Immediatetly shutdown if underpressure is detected
                emit new_state("Shutdown Ph. 1");
                return;
            }
        } catch (...) {
            cout << "Could not access data with ID: P4" << endl;
        }
    }
}

void State_Machine::update_signals(bool new_state) {
    if ("hotfire_1" == this->config_name) {
        this->hotfire_1(new_state);
    } else {
        cout << "State Machine: Unknown configuration: " << this->config_name.toStdString() << endl;
    }
}
