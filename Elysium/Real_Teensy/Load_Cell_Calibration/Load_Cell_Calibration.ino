/*
-------------------------------------------------------------------
Load Cell Calibration
-------------------------------------------------------------------
Program for the Teensy to output readings from 1-3 load cells and 
report the results for calibration/verification.
*/

/*
-------------------------------------------------------------------
VARIABLES & USER INPUT
-------------------------------------------------------------------
*/

// time variables
long unsigned sensor_update_last = 0;
long unsigned sensor_update_interval = 100000;    // sensor update interval (microsec)     <-- USER INPUT

// BAUD rate 
const int BAUD = 115200;                   // serial com in bits per second     <-- USER INPUT

/*
LOAD CELL SET UP
----------------
*/

// import library
#include "HX711.h"

// define data and clock pins for each loadcell
const int LC1_D_OUT_PIN = -1;           // <-- USER INPUT
const int LC1_CLK_PIN   = -1;           // <-- USER INPUT
const int LC2_D_OUT_PIN = -1;           // <-- USER INPUT
const int LC2_CLK_PIN   = -1;           // <-- USER INPUT
const int LC3_D_OUT_PIN = -1;           // <-- USER INPUT
const int LC3_CLK_PIN   = -1;           // <-- USER INPUT

// measurement set up
float weight1;
float weight2;
float weight3;
HX711 scale1, scale2, scale3;

/*
-------------------------------------------------------------------
SETUP LOOP
-------------------------------------------------------------------
*/
void setup() {
  Serial.begin(BAUD);           // initializes serial communication at set baud rate
  
  /*
  LOAD CELL SET UP
  -----------------------
  */

  // load cell setup
  scale1.begin(LC1_D_OUT_PIN, LC1_CLK_PIN);
  scale2.begin(LC2_D_OUT_PIN, LC2_CLK_PIN);
  scale3.begin(LC3_D_OUT_PIN, LC3_CLK_PIN);

  // scale and tare load cells
  scale1.set_scale(2280.f);  // Set the scale factor for conversion to kilograms
  scale1.tare();             // Reset the scale to zero

  scale2.set_scale(2280.f); // Set the scale factor for conversion to kilograms
  scale2.tare();            // Reset the scale to zero
  
  scale3.set_scale(2280.f); // Set the scale factor for conversion to kilograms
  scale3.tare();            // Reset the scale to zero
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


    // measure force from each load cell
    weight1 = scale1.get_units(1); // Get the weight in kilograms
    weight2 = scale2.get_units(1); // Get the weight in kilograms
    weight3 = scale3.get_units(1); // Get the weight in kilograms

    // send data to serial monitor 
    Serial.print("t:"); Serial.print(sensor_update_last);                                  // print time reading
    
    // If the load cell has been defined, print the output.
    // Values printed in kg
    if (LC1_D_OUT_PIN >= 0) {
      Serial.print(",L1:"); Serial.print(weight1);
    } if (LC2_D_OUT_PIN >= 0) {
      Serial.print(",L2:"); Serial.print(weight2);
    } if (LC3_D_OUT_PIN >= 0) {
      Serial.print(",L3:"); Serial.print(weight3);
    }
    Serial.println();
    delay(10);
  }

}