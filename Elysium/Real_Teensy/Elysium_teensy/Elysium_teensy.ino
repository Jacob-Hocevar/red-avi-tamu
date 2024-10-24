/*
-------------------------------------------------------------------
VARIABLES & USER INPUT
-------------------------------------------------------------------
*/

#include <Arduino.h>

// time variables
long unsigned sensor_update_last = 0;
long unsigned sensor_update_interval = 100000;    // sensor update interval (microsec)     <-- USER INPUT

// variable for how long program should wait before shutting system down
long unsigned shutdown_time = 1000000;                                       // <-- USER INPUT
long unsigned last_update_time = 0;

// BAUD rate 
const int BAUD = 115200;                   // serial com in bits per second     <-- USER INPUT

// LABV 1 state variable
bool is_LABV1_open = false;

/*
VALVE SETUP
*/

// Valves
const int NCS1_pin = 0;           // <-- USER INPUT
const int NCS2_pin = 0;           // <-- USER INPUT
const int NCS4_pin = 0;           // <-- USER INPUT
const int LABV1_pin = 0;          // <-- USER INPUT
const int LABV2_pin = 0;          // <-- USER INPUT

// Igniter
const int Igniter1_pin = 0;
const int Igniter2_pin = 0;
const int Igniter3_pin = 0;

// serial input variables
String ln1 = "";
String ln2 = "";
String valveName = "";

const int valve_count = 9;  // Total number of valves (NCS1, NCS2, NCS4, LABV1, LABV2, Igniter1, Igniter2, Igniter3)

enum Valve {
    NCS1,
    NCS2,
    NCS4,
    LABV1,
    LABV2,
    IGNITER1,
    IGNITER2,
    IGNITER3
};

// Array of valve pin numbers
const int valvePins[valve_count] = {
    NCS1_pin,  // Pin number for NCS1
    NCS2_pin,  // Pin number for NCS2
    NCS4_pin,  // Pin number for NCS4
    LABV1_pin, // Pin number for LABV1
    LABV2_pin, // Pin number for LABV2
    Igniter1_pin, // Pin number for Igniter1
    Igniter2_pin, // Pin number for Igniter2
    Igniter3_pin, // Pin number for Igniter3
};

/*
LOAD CELL SET UP
----------------
*/

// import library
#include "HX711.h"

// define data and clock pins for each loadcell
const int LC1_D_OUT_PIN = 0;            // <-- USER INPUT
const int LC1_CLK_PIN = 0;              // <-- USER INPUT
const int LC2_D_OUT_PIN2 = 0;           // <-- USER INPUT
const int LC2_CLK_PIN2 = 0;             // <-- USER INPUT

// measurement set up
float weight1;
float weight2;
HX711 scale, scale2;



/*
THERMOCOUPLE SET UP
----------------
*/

// Thermocouple libraries
#include <Wire.h>
#include <Adafruit_MCP9600.h>

// Thermocouple I2C addresses
#define I2C_ADDRESS1 (0x67)
#define I2C_ADDRESS2 (0x60)

// Thermocouple mcp identifier
Adafruit_MCP9600 mcp;
Adafruit_MCP9600 mcp2;



/*
PRESSURE TRANSDUCER SET UP
----------------
*/

// teensy pins to read signals
const int pt1_pin = 0;                    // <-- USER INPUT
const int pt2_pin = 0;                    // <-- USER INPUT
const int pt3_pin = 0;                    // <-- USER INPUT
const int pt4_pin = 0;                    // <-- USER INPUT
const int pt5_pin = 0;                    // <-- USER INPUT

// analog and digital reading variables setup
int pt1_analog = 0;                        // analog reading from PT output signal
int pt2_analog = 0;                        // analog reading from PT output signal
int pt3_analog = 0;                        // analog reading from PT output signal
int pt4_analog = 0;                        // analog reading from PT output signal
int pt5_analog = 0;                        // analog reading from PT output signal

const float pt_slope = 0;
const float pt_intercept = 0;

// Function to calculate pressure
float pressureCalculation(float analog) {
    // Calculate pressure based on analog input
    float pressure = pt_slope * analog + pt_intercept;
    return pressure;  // Return the calculated pressure
}

/*
ACCELEROMETER SET UP
----------------
*/

// import libraries
#include "IMU.h"

#include "LSM6DSL.h"

#include "LIS3MDL.h"

// measurement variable setup
byte buff[6];
int accRaw[3];
float accx, accy, accz;
const float accoffset = 4180;


/*
-------------------------------------------------------------------
SETUP LOOP
-------------------------------------------------------------------
*/
void setup() {
  Serial.begin(BAUD);           // initializes serial communication at set baud rate
  Wire.begin();

  /*
  VALVE SET UP
  -----------------------
  */

  pinMode(NCS1_pin, OUTPUT);    // sets the digital pin as output for controlling Valve 1 MOSFET
  pinMode(NCS2_pin, OUTPUT);    // sets the digital pin as output for controlling Valve 2 MOSFET
  pinMode(NCS4_pin, OUTPUT);    // sets the digital pin as output for controlling Valve 4 MOSFET
  pinMode(LABV1_pin, OUTPUT);    // sets the digital pin as output for controlling Valve 5 MOSFET
  pinMode(LABV2_pin, OUTPUT);    // sets the digital pin as output for controlling Valve 6 MOSFET

  /*
  THERMOCOUPLE SET UP
  -----------------------
  */

  // Initialize MCP9600 sensors
  mcp.begin(I2C_ADDRESS1);
  mcp2.begin(I2C_ADDRESS2);

  // Configure both sensors
  mcp.setADCresolution(MCP9600_ADCRESOLUTION_18);
  mcp.setThermocoupleType(MCP9600_TYPE_K);
  mcp.setFilterCoefficient(3);
  mcp.enable(true);

  mcp2.setADCresolution(MCP9600_ADCRESOLUTION_18);
  mcp2.setThermocoupleType(MCP9600_TYPE_K);
  mcp2.setFilterCoefficient(3);
  mcp2.enable(true);

  /*
  LOAD CELL SET UP
  -----------------------
  */

  // load cell setup
  scale.begin(LC1_D_OUT_PIN, LC1_CLK_PIN);
  scale2.begin(LC2_D_OUT_PIN2, LC2_CLK_PIN2);

  // scale and tare load cells
  scale.set_scale(2280.f);  // Set the scale factor for conversion to kilograms
  scale.tare();             // Reset the scale to zero

  scale2.set_scale(2280.f); // Set the scale factor for conversion to kilograms
  scale2.tare();            // Reset the scale to zero
  
  /*
  ACCELEROMETER SET UP
  -----------------------
  */

  // detect and enable IMU
  detectIMU();
  enableIMU();
}

/*
-------------------------------------------------------------------
LOOP
-------------------------------------------------------------------
*/
void loop() {
  // check for last reading update
  if ((micros() - sensor_update_last) > sensor_update_interval) {
    sensor_update_last = micros();                               // update last time update

    // read pressure data
    pt1_analog = analogRead(pt1_pin);                          // reads value from input pin and assigns to variable
    pt2_analog = analogRead(pt2_pin);                          // reads value from input pin and assigns to variable.
    pt3_analog = analogRead(pt3_pin);
    pt4_analog = analogRead(pt4_pin);
    pt5_analog = analogRead(pt5_pin);

    // read acceleration
    readACC(buff);
    accRaw[0] = (int16_t)(buff[0] | (buff[1] << 8)); 
    accRaw[1] = (int16_t)(buff[2] | (buff[3] << 8));
    accRaw[2] = (int16_t)(buff[4] | (buff[5] << 8));
    accx = accRaw[0]/accoffset;
    accy = accRaw[1]/accoffset;
    accz = accRaw[2]/accoffset;

    // measure force from each load cell
    weight1 = scale.get_units(1);  // Get the weight in kilograms
    weight2 = scale2.get_units(1); // Get the weight in kilograms

    // send data to serial monitor 
    Serial.print("t:"); Serial.print(sensor_update_last);          // print time reading
    Serial.print(",P1:"); Serial.print(pressureCalculation(pt1_analog));              // print pressure calculation in psi
    Serial.print(",P2:"); Serial.print(pressureCalculation(pt2_analog));              // print pressure calculation in psi
    Serial.print(",P3:"); Serial.print(pressureCalculation(pt3_analog));              // print pressure calculation in psi
    Serial.print(",P4:"); Serial.print(pressureCalculation(pt4_analog));              // print pressure calculation in psi
    Serial.print(",P5:"); Serial.print(pressureCalculation(pt5_analog));              // print pressure calculation in psi
    Serial.print(",T1:"); Serial.print(mcp.readThermocouple());    // print thermocouple temperature in C
    Serial.print(",T2:"); Serial.print(mcp2.readThermocouple());   // print thermocouple temperature in C
    Serial.print(",L1:"); Serial.print(weight1);                   // print load cell 1 in kg
    Serial.print(",L2:"); Serial.print(weight2);                   // print load cell 2 in kg
    Serial.print(",Ax:"); Serial.print(accx);                      // print acceleration in x direction  <-- determine what direction x is in relation to engine
    Serial.print(",Ay:"); Serial.print(accy);                      // print acceleration in y direction  <-- determine what direction y is in relation to engine
    Serial.print(",Az:"); Serial.print(accz);                      // print acceleration in z direction  <-- determine what direction z is in relation to engine
    Serial.println();
    delay(10);
  }

  // checks if user input is available to read
  if (Serial.available() > 0) {
    // read user input
    String input_signal = Serial.readStringUntil("\n");
    char delimiter = ':';
    int delimiterIndex = input_signal.indexOf(delimiter);

    // break string into identifier and control state
    if (delimiterIndex != -1) {
      ln1 = input_signal.substring(0, delimiterIndex);
      ln2 = input_signal.substring(delimiterIndex + 1).toInt();
    }

    // Map string input to enum
    Valve valveToControl;
    if (valveName == "NCS1") valveToControl = NCS1;
    else if (ln1 == "NCS2") valveToControl = NCS2;
    else if (ln1 == "NCS4") valveToControl = NCS4;
    else if (ln1 == "LA-BV1") valveToControl = LABV1;
    else if (ln1 == "LA-BV2") valveToControl = LABV2;
    else if (ln1 == "IGNITER1") valveToControl = IGNITER1;
    else if (ln1 == "IGNITER2") valveToControl = IGNITER2;
    else if (ln1 == "IGNITER3") valveToControl = IGNITER3;
    else {
      Serial.println("not recognized");
    }
    
    // update last time of communication variable
    last_update_time = micros();

    // convert input to corresponding state, actuate pins, and set valve control state
    switch (ln2) {
      case 0:
        digitalWrite(valvePins[valveToControl], LOW);
        break;
      case 1:
        digitalWrite(valvePins[valveToControl], HIGH);
        break;
      default:
        Serial.println("Unknown");
        break;
    }

/*
    // Normally closed solenoid valve 1
    if (input == "NCS1:0\r\n") {
      digitalWrite(NCS1_pin, LOW); // Open
    }
    if (input == "NCS1:1\r\n") {
      digitalWrite(NCS1_pin, HIGH);  // Closed
    }

    // Normally closed solenoid valve 2
    if (input == "NCS2:0\r\n") {
      digitalWrite(NCS2_pin, LOW);
    }
    if (input == "NCS2:1\r\n") {
      digitalWrite(NCS2_pin, HIGH);
    }

    // Normally closed solenoid valve 4
    if (input == "NCS4:0\r\n") {
      digitalWrite(NCS4_pin, LOW);
    }
    if (input == "NCS4:1\r\n") {
      digitalWrite(NCS4_pin, HIGH);
    }

    // Linearly Actuated Ball Valve 1
    if (input == "LA-BV 1:0\r\n") {
      digitalWrite(LABV1_pin, LOW);
      is_LABV1_open = true;
    }
    if (input == "LA-BV 1:1\r\n") {
      digitalWrite(LABV1_pin, HIGH);
      is_LABV1_open = false;
    }

    // Linearly Actuated Ball Valve 2
    if (input == "LA-BV 2:0\r\n") {
      digitalWrite(LABV2_pin, LOW);
    }
    if (input == "LA-BV 2:1\r\n") {
      digitalWrite(LABV2_pin, HIGH);
    }

    // Igniter 1
    if (input == "IGNITE:1\r\n") {
      digitalWrite(Igniter_pin, HIGH);
    }

    // Igniter 2
    if (input == "IGNITE:1\r\n") {
      digitalWrite(Igniter_pin, HIGH);
    }

    // Igniter 3
    if (input == "IGNITE:1\r\n") {
      digitalWrite(Igniter_pin, HIGH);
    }
    */
  }

  // Shutdown engine if teensy is not communicating with computer
  if ((micros() - last_update_time) > shutdown_time) {
    // Shutdown engine

    // Open NCS2
    digitalWrite(NCS2_pin, HIGH);

    // Close ball valves & NCS1
    digitalWrite(NCS1_pin, LOW);
    digitalWrite(LABV1_pin, LOW);
    digitalWrite(LABV2_pin, LOW);

    if (is_LABV1_open) {
      // Open NCS4 for 3 seconds
      digitalWrite(NCS4_pin, HIGH);
      delay(3000);
      digitalWrite(NCS4_pin, LOW);
    }

    // End program
    while(1);
  }
}