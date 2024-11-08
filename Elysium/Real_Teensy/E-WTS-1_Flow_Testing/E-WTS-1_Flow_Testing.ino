/*
-------------------------------------------------------------------
TIME AND COMMUNICATION SETUP
-------------------------------------------------------------------
*/

// time variables
long unsigned sensor_update_last = 0;
long unsigned connection_last = 0;
const long unsigned SENSOR_UPDATE_INTERVAL = 50000;   // Time between remeasuring sensors (microsec)               <-- USER INPUT
const long unsigned CONNECTION_TIMEOUT = 200000;      // Time without comms before automated shutdown (microsec)   <-- USER INPUT
const long unsigned TIMEOUT_PRINT_INTERVAL = 500000;  // Time betweeen alerts when comms are out (microsec)        <-- USER INPUT

// BAUD rate 
const int BAUD = 115200;    // serial com in bits per second     <-- USER INPUT

/*
-------------------------------------------------------------------
VALVE SETUP
-------------------------------------------------------------------
*/

// Valve pins
const int NCS1_PIN = 9;           // <-- USER INPUT
const int NCS2_PIN = 10;          // <-- USER INPUT
const int LABV1_PIN = 7;          // <-- USER INPUT
const int LABV2_PIN = 8;          // <-- USER INPUT

// Function to get pin number from string
int get_pin(String id) {
  if (id == "NCS1") {
    return NCS1_PIN;
  } else if (id =="NCS2") {
    return NCS2_PIN;
  } else if (id =="LA-BV1") {
    return LABV1_PIN;
  } else if (id =="LA-BV2") {
    return LABV2_PIN;
  }

  // Return -1 if no match was found
  return -1;
}

/*
-------------------------------------------------------------------
PRESSURE TRANSDUCER SET UP
-------------------------------------------------------------------
*/

// teensy pins to read signals
const int PT1_PIN = 45;                    // <-- USER INPUT
const int PT2_PIN = 44;                    // <-- USER INPUT

int pt1_analog = 0;                        // analog reading from PT output signal
int pt2_analog = 0;                        // analog reading from PT output signal

// Calibration constants
const float PT_SLOPES[] = {237.5, 237.5};           // <-- USER INPUT
const float PT_INTERCEPTS[] = {-68.75, -68.75};     // <-- USER INPUT

// Function to calculate pressure
float get_pressure(float analog_reading, size_t id) {
  // Convert Voltage reading to measurement in psi with the correct calibration factors
  // PTs are 1-indexed, the lists are 0-indexed
  return PT_SLOPES[id-1] * analog_reading + PT_INTERCEPTS[id-1];
}

/*
-------------------------------------------------------------------
FLOW METER SET UP
-------------------------------------------------------------------
*/

// Configuration
const int FM_PIN = 43;            // Digital pin connected to the flow meter    <-- USER INPUT
const float VOL_PER_PULSE = 2.5;  // Volume per pulse in mL
volatile int pulse_count = 0;     // Counter for pulses

// Interrupt service routine to handle the flow meter pulse
void count_pulse() {
  pulse_count++;
}

/*
-------------------------------------------------------------------
THERMOCOUPLE SET UP
-------------------------------------------------------------------
*/

// Enable/Disable Thermocouple for testing
const bool USING_THERMO = false;

// Thermocouple libraries
#include <Wire.h>
#include <Adafruit_MCP9600.h>

// Thermocouple I2C addresses
#define I2C_ADDRESS1 (0x67)

// Thermocouple mcp identifier
Adafruit_MCP9600 mcp;

void setup() {
  // Initialize serial communication as configured and start thermocouple library
  Serial.begin(BAUD);
  Wire.begin();

  // VALVES
  // Configure digital pins for actuating valves as output only
  pinMode(NCS1_PIN, OUTPUT);
  pinMode(NCS2_PIN, OUTPUT);
  pinMode(LABV1_PIN, OUTPUT);
  pinMode(LABV2_PIN, OUTPUT);

  // FLOW METER
  // Set digital pin as input for the flow meter
  pinMode(FM_PIN, INPUT);

  // Attach an interrupt to the flow meter pin
  // RISING means the interrupt will trigger when the signal changes from LOW to HIGH
  // FALLING means the interrupt will trigger when the signal changes from HIGH to LOW
  // CHANGE means the interrupt will trigger for a change in either direction
  attachInterrupt(digitalPinToInterrupt(FM_PIN), count_pulse, CHANGE);

  // THERMOCOUPLE
  // Only initialize if enabled
  if (USING_THERMO) {
    // Initialize MCP9600 sensors
    mcp.begin(I2C_ADDRESS1);

    // Configure sensors
    mcp.setADCresolution(MCP9600_ADCRESOLUTION_18);
    mcp.setThermocoupleType(MCP9600_TYPE_K);
    mcp.setFilterCoefficient(3);
    mcp.enable(true);
  }
}

void loop() {
  /*
  -------------------------------------------------------------------
  SENSOR READING
  -------------------------------------------------------------------
  */

  // Check how long it has been since the last measurement
  if ((micros() - sensor_update_last) > SENSOR_UPDATE_INTERVAL) {
    // Update the time of last reading
    sensor_update_last = micros();

    // Read Pressure Transducer analog data
    pt1_analog = analogRead(PT1_PIN);
    pt2_analog = analogRead(PT2_PIN);

    // Print sensor readings to the serial output
    // Convert pressures to psi
    Serial.print("P1:"); Serial.print(get_pressure(pt1_analog, 1));
    Serial.print(",P2:"); Serial.print(get_pressure(pt2_analog, 2));

    // Only read/print thermocouple readings if enabled
    if (USING_THERMO) {
      // Outputs in °C
      Serial.print(",T1:"); Serial.print(mcp.readThermocouple());
    }

    // Convert number of pulses to total volume and print flow meter readings
    Serial.print(",V1:"); Serial.print(VOL_PER_PULSE * pulse_count);

    // End the line to signify the end of message
    Serial.println();
  }

  /*
  -------------------------------------------------------------------
  VALVE ACTUATION
  -------------------------------------------------------------------
  */

  if (Serial.available() > 0) {
    // Update the time since last communication
    connection_last = micros();

    // read communication until end of message
    String input = Serial.readStringUntil('\n');

    // nop is the no operation command, which is merely to confirm that the connection is active
    if (input == "nop\r") { return; }

    // Parse message into ID and value, convert to pin number and an int
    // If message cannot be split, it is meaningless
    int split_index = input.indexOf(':');
    if (-1 == split_index) { return; }

    // If the pin number cannot be resolved, it is meaningless
    int pin = get_pin(input.substring(0, split_index));
    if (-1 == pin) { return; }

    // Convert the second half of the string to the integer value
    int value = input.substring(split_index + 1).toInt();
  

    // Use the pin and value to actuate the specified valve
    switch (value) {
      case 0:
        digitalWrite(pin, LOW);   // Close Valve
        break;
      case 1:
        digitalWrite(pin, HIGH);  // Open Valve
    }
  }

  /*
  -------------------------------------------------------------------
  COMMUNICATIONS LOSS CHECK
  -------------------------------------------------------------------
  */
  // Check if the time since last communication exceeds the allowable timeout
  if ((micros() - connection_last) > CONNECTION_TIMEOUT) {
    // Guarantee a return to a safe state by closing all valves
    digitalWrite(NCS1_PIN, LOW);
    digitalWrite(NCS2_PIN, LOW);
    digitalWrite(LABV1_PIN, LOW);
    digitalWrite(LABV2_PIN, LOW);

    // Enter a fixed loop until connection is reestablished and reinitiated
    bool connection_lost = true;
    long unsigned print_last = 0;
    while (connection_lost) {
      // Every TIMEOUT_PRINT_INTERAVAL, output the aborted message to alert the operator of the current state
      if ((micros() - print_last) > TIMEOUT_PRINT_INTERVAL) {
        print_last = micros();
        Serial.println("Aborted");
      }

      // Check for input from the operator
      if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        Serial.println("New Input= " + input);

        // Only respond if the specific string is encountered
        if (input == "Start\r") {
          // Update timer
          connection_last = micros();

          // Exit the connection_lost loop
          connection_lost = false;
        }
      }
    }
  }
}
