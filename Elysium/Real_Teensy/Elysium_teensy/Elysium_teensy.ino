/*
-------------------------------------------------------------------
Elysium Teensy
-------------------------------------------------------------------
(Add text for what this program does)
*/

/*
-------------------------------------------------------------------
VARIABLES & USER INPUT
-------------------------------------------------------------------
*/

// time variables
long unsigned sensor_update_last = 0;
long unsigned sensor_update_interval = 100000;    // sensor update interval (microsec)     <-- USER INPUT

// variable for how long program should wait before shutting system down
long unsigned shutdown_time = 1000000;                                       // <-- USER INPUT
long unsigned last_update_time = 0;

// BAUD rate 
const int BAUD = 115200;                   // serial com in bits per second     <-- USER INPUT




/*
VALVE SETUP
*/

// Valves
const int NCS1_pin = 0;           // <-- USER INPUT
const int NCS2_pin = 0;           // <-- USER INPUT
const int NCS3_pin = 0;           // <-- USER INPUT
const int NCS4_pin = 0;           // <-- USER INPUT
const int LABV1_pin = 0;           // <-- USER INPUT
const int LABV2_pin = 0;           // <-- USER INPUT

// Igniter
const int Igniter_pin = 0;           // <-- USER INPUT



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


// callibration parameters for the pts
const float pt1_ref1 = 0;                       // known pressure low                <-- USER INPUT
const float pt1_ref2 = 0 + pt1_ref1;            // known pressure high               <-- USER INPUT
const float pt2_ref1 = 0;                       // known pressure low                <-- USER INPUT
const float pt2_ref2 = 0 + pt2_ref1;            // known pressure high               <-- USER INPUT
const float pt3_ref1 = 0;                       // known pressure low                <-- USER INPUT
const float pt3_ref2 = 0 + pt3_ref1;            // known pressure high               <-- USER INPUT
const float pt4_ref1 = 0;                       // known pressure low                <-- USER INPUT
const float pt4_ref2 = 0 + pt4_ref1;            // known pressure high               <-- USER INPUT
const float pt5_ref1 = 0;                       // known pressure low                <-- USER INPUT
const float pt5_ref2 = 0 + pt5_ref1;            // known pressure high               <-- USER INPUT

const int pt1_analog_ref1 = 0;         // analog at known pressure small    <-- USER INPUT
const int pt1_analog_ref2 = 0;         // analog at known pressure high     <-- USER INPUT
const int pt2_analog_ref1 = 0;         // analog at known pressure small    <-- USER INPUT
const int pt2_analog_ref2 = 0;         // analog at known pressure high     <-- USER INPUT
const int pt3_analog_ref1 = 0;         // analog at known pressure small    <-- USER INPUT
const int pt3_analog_ref2 = 0;         // analog at known pressure high     <-- USER INPUT
const int pt4_analog_ref1 = 0;         // analog at known pressure small    <-- USER INPUT
const int pt4_analog_ref2 = 0;         // analog at known pressure high     <-- USER INPUT
const int pt5_analog_ref1 = 0;         // analog at known pressure small    <-- USER INPUT
const int pt5_analog_ref2 = 0;         // analog at known pressure high     <-- USER INPUT

// analog and digital reading variables setup
int pt1_analog = 0;                        // analog reading from PT output signal
int pt2_analog = 0;                        // analog reading from PT output signal
int pt3_analog = 0;                        // analog reading from PT output signal
int pt4_analog = 0;                        // analog reading from PT output signal
int pt5_analog = 0;                        // analog reading from PT output signal
float pt1_pressure = 0.0;                  // calculated pressure using calibration eq
float pt2_pressure = 0.0;                  // calculated pressure using calibration eq
float pt3_pressure = 0.0;                  // calculated pressure using calibration eq
float pt4_pressure = 0.0;                  // calculated pressure using calibration eq
float pt5_pressure = 0.0;                  // calculated pressure using calibration eq

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
  pinMode(NCS3_pin, OUTPUT);    // sets the digital pin as output for controlling Valve 3 MOSFET
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

    // Calculate pressure value
    pt1_pressure = pt1_ref1 + (pt1_ref2 - pt1_ref1)/(pt1_analog_ref2 - pt1_analog_ref1)*(pt1_analog - pt1_analog_ref1); // calculates pressure reading using calibration parameters
    pt2_pressure = pt2_ref1 + (pt2_ref2 - pt2_ref1)/(pt2_analog_ref2 - pt2_analog_ref1)*(pt2_analog - pt2_analog_ref1); // calculates pressure reading using calibration parameters
    pt3_pressure = pt3_ref1 + (pt3_ref2 - pt3_ref1)/(pt3_analog_ref2 - pt3_analog_ref1)*(pt3_analog - pt3_analog_ref1); // calculates pressure reading using calibration parameters
    pt4_pressure = pt4_ref1 + (pt4_ref2 - pt4_ref1)/(pt4_analog_ref2 - pt4_analog_ref1)*(pt4_analog - pt4_analog_ref1); // calculates pressure reading using calibration parameters
    pt5_pressure = pt5_ref1 + (pt5_ref2 - pt5_ref1)/(pt5_analog_ref2 - pt5_analog_ref1)*(pt5_analog - pt5_analog_ref1); // calculates pressure reading using calibration parameters

    // measure acceleration
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
    Serial.print(sensor_update_last);                                  // print time reading
    // Serial.print(","); Serial.print(pt1_analog);             // print analog pressure reading (comment out when using GUI)
    // Serial.print(","); Serial.print(pt2_analog);             // print analog pressure reading (comment out when using GUI)
    Serial.print("P1:"); Serial.print(pt1_pressure);             // print pressure calculation in psi
    Serial.print(",P2:"); Serial.print(pt2_pressure);              // print pressure calculation in psi
    Serial.print(",P3:"); Serial.print(pt2_pressure);              // print pressure calculation in psi
    Serial.print(",P4:"); Serial.print(pt2_pressure);              // print pressure calculation in psi
    Serial.print(",P5:"); Serial.print(pt2_pressure);              // print pressure calculation in psi
    Serial.print(",T1:"); Serial.print(mcp.readThermocouple());    // print thermocouple temperature in C
    Serial.print(",T2:"); Serial.print(mcp2.readThermocouple());    // print thermocouple temperature in C
    Serial.print(",L1:"); Serial.print(weight1);                   // print load cell 1 in kg
    Serial.print(",L2:"); Serial.print(weight2);                   // print load cell 2 in kg
    Serial.print(",x:"); Serial.print(accx);                      // print acceleration in x direction  <-- determine what direction x is in relation to engine
    Serial.print(",y:"); Serial.print(accy);                      // print acceleration in y direction  <-- determine what direction y is in relation to engine
    Serial.print(",z:"); Serial.print(accz);                      // print acceleration in z direction  <-- determine what direction z is in relation to engine
    Serial.println();
    delay(10);
  }

  // checks if user input is available to read
  if (Serial.available() > 0) {
    // read user input
    String input = Serial.read();

    // update last time of communication variable
    last_update_time = micros();

    // convert input to corresponding state, actuate pins, and set valve control state

    // Normally closed solenoid valve 1
    if (input == "NCS1:0\r\n") {
      digitalWrite(NCS1_pin, HIGH); // Open
    }
    if (input == "NCS1:1\r\n") {
      digitalWrite(NCS1_pin, LOW);  // Closed
    }

    // Normally closed solenoid valve 2
    if (input == "NCS2:0\r\n") {
      digitalWrite(NCS2_pin, HIGH);
    }
    if (input == "NCS2:1\r\n") {
      digitalWrite(NCS2_pin, LOW);
    }

    // Normally closed solenoid valve 3
    if (input == "NCS3:0\r\n") {
      digitalWrite(NCS3_pin, HIGH);
    }
    if (input == "NCS3:1\r\n") {
      digitalWrite(NCS3_pin, LOW);
    }

    // Normally closed solenoid valve 4
    if (input == "NCS4:0\r\n") {
      digitalWrite(NCS4_pin, HIGH);
    }
    if (input == "NCS4:1\r\n") {
      digitalWrite(NCS4_pin, LOW);
    }

    // Linearly Actuated Ball Valve 1
    if (input == "LA-BV 1:0\r\n") {
      digitalWrite(LABV1_pin, HIGH);
    }
    if (input == "LA-BV 1:1\r\n") {
      digitalWrite(LABV1_pin, LOW);
    }

    // Linearly Actuated Ball Valve 2
    if (input == "LA-BV 2:0\r\n") {
      digitalWrite(LABV2_pin, HIGH);
    }
    if (input == "LA-BV 2:1\r\n") {
      digitalWrite(LABV2_pin, LOW);
    }

    // Igniter
    if (input == "IGNITE:1\r\n") {
      digitalWrite(Igniter_pin, HIGH);
    }
  }

  // Shutdown engine if teensy is not communicating with computer
  if ((micros() - last_update_time) > shutdown_time) {
    // Shutdown engine

    // Open NCS2 & NCS3
    digitalWrite(NCS2_pin, HIGH);
    digitalWrite(NCS3_pin, HIGH);

    // Close ball valves & NCS1
    digitalWrite(NCS1_pin, LOW);
    digitalWrite(LABV1_pin, LOW);
    digitalWrite(LABV2_pin, LOW);

    // Open NCS4 for 3 seconds
    digitalWrite(NCS4_pin, HIGH);
    delay(3000);
    digitalWrite(NCS4_pin, LOW);

    // End program
    abort(); // maybe break command? Maybe while?
    while(1);
  }
}