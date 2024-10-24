/*
-------------------------------------------------------------------
VARIABLES & USER INPUT
-------------------------------------------------------------------
*/

// BAUD rate 
const int BAUD = 115200;                   // serial com in bits per second     <-- USER INPUT

/*
-------------------------------------------------------------------
VALVE SETUP
-------------------------------------------------------------------
*/

// Valve pins
const int NCS1_pin = 0;           // <-- USER INPUT
const int NCS2_pin = 0;           // <-- USER INPUT
const int LABV1_pin = 0;          // <-- USER INPUT
const int LABV2_pin = 0;          // <-- USER INPUT

// Igniter pins
const int igniter1 = 0;           // <-- USER INPUT
const int igniter2 = 0;           // <-- USER INPUT
const int igniter3 = 0;           // <-- USER INPUT

/*
-------------------------------------------------------------------
PRESSURE TRANSDUCER SET UP
-------------------------------------------------------------------
*/

// teensy pins to read signals
const int pt1_pin = 0;                    // <-- USER INPUT
const int pt2_pin = 0;                    // <-- USER INPUT

int pt1_analog = 0;                        // analog reading from PT output signal
int pt2_analog = 0;                        // analog reading from PT output signal

const float pt_slope = 0;                    // <-- USER INPUT
const float pt_intercept = 0;                // <-- USER INPUT

// Function to calculate pressure
float pressureCalculation(float analog) {
    // Calculate pressure based on analog input
    float pressure = pt_slope * analog + pt_intercept;
    return pressure;  // Return the calculated pressure
}

// analog and digital reading variables setup
int pt1_analog = 0;                        // analog reading from PT output signal
int pt2_analog = 0;                        // analog reading from PT output signal

void setup() {
  Serial.begin(BAUD);           // initializes serial communication at set baud rate
  Wire.begin();

  /*
  -------------------------------------------------------------------
  VALVE SET UP
  -------------------------------------------------------------------
  */

  pinMode(NCS1_pin, OUTPUT);    // sets the digital pin as output for controlling Valve 1 MOSFET
  pinMode(NCS2_pin, OUTPUT);    // sets the digital pin as output for controlling Valve 2 MOSFET
  pinMode(LABV1_pin, OUTPUT);    // sets the digital pin as output for controlling Valve 5 MOSFET
  pinMode(LABV2_pin, OUTPUT);    // sets the digital pin as output for controlling Valve 6 MOSFET
  pinMode(igniter1, OUTPUT);
  pinMode(igniter2, OUTPUT);
  pinMode(igniter3, OUTPUT);
}

void loop() {
  /*
  -------------------------------------------------------------------
  SENSOR READING
  -------------------------------------------------------------------
  */
  pt1_analog = analogRead(pt1_pin);                          // reads value from input pin and assigns to variable
  pt2_analog = analogRead(pt2_pin);                          // reads value from input pin and assigns to variable.

  // Serially print sensor readings
  Serial.print("P1:"); Serial.print(pressureCalculation(pt1_analog));              // print pressure calculation in psi
  Serial.print(",P2:"); Serial.print(pressureCalculation(pt2_analog));              // print pressure calculation in psi

  /*
  -------------------------------------------------------------------
  VALVE ACTUATION
  -------------------------------------------------------------------
  */

if (Serial.available() > 0) {
  // read signal
  String input = Serial.readStringUntil("\n");

  // Normally closed solenoid valve 1
  if (input == "NCS1:0\r\n") {
    digitalWrite(NCS1_pin, LOW);
  }
  if (input == "NCS1:1\r\n") {
    digitalWrite(NCS1_pin, HIGH);
  }

  // Normally closed solenoid valve 2
  if (input == "NCS2:0\r\n") {
    digitalWrite(NCS2_pin, LOW);
  }
  if (input == "NCS2:1\r\n") {
    digitalWrite(NCS2_pin, HIGH);
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
  }
}