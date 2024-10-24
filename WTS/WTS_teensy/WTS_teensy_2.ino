
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
-------------------------------------------------------------------
VALVE SETUP
-------------------------------------------------------------------
*/

// Valve pins
const int NCS1_pin = 0;           // <-- USER INPUT
const int NCS2_pin = 0;           // <-- USER INPUT
const int LABV1_pin = 0;          // <-- USER INPUT
const int LABV2_pin = 0;          // <-- USER INPUT

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
const float pt_intercept = 0;                    // <-- USER INPUT

// Function to calculate pressure
float pressureCalculation(float analog) {
    // Calculate pressure based on analog input
    float pressure = pt_slope * analog + pt_intercept;
    return pressure;  // Return the calculated pressure
}

// analog and digital reading variables setup
int pt1_analog = 0;                        // analog reading from PT output signal
int pt2_analog = 0;                        // analog reading from PT output signal

/*
-------------------------------------------------------------------
FLOW METER SET UP
-------------------------------------------------------------------
*/

// Constants
const int flowMeterPin = 0;   // Digital pin connected to the flow meter
const float mlPerPulse = 2.5; // Volume per pulse in mL
volatile int pulseCount = 0;  // Counter for pulse counts

// Interrupt service routine to handle the flow meter pulse
void countPulse() {
  pulseCount++;
}

// Number of pulse counts until sending flow rate
int pulseReading = 20;

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

  /*
  -------------------------------------------------------------------
  FLOW METER SET UP
  -------------------------------------------------------------------
  */

  // Set up the flow meter pin as an input
  pinMode(flowMeterPin, INPUT);   // sets the digital pin as input for flow meter

  // Attach an interrupt to the flow meter pin
  // RISING means the interrupt will trigger on the rising edge of the pulse
  attachInterrupt(digitalPinToInterrupt(flowMeterPin), countPulse, RISING);
}

void loop() {
  /*
  -------------------------------------------------------------------
  SENSOR READING
  -------------------------------------------------------------------
  */

  // check for last reading update
  if ((micros() - sensor_update_last) > sensor_update_interval) {
    sensor_update_last = micros();                               // update last time update

    pt1_analog = analogRead(pt1_pin);                          // reads value from input pin and assigns to variable
    pt2_analog = analogRead(pt2_pin);                          // reads value from input pin and assigns to variable.

    // Serially print sensor readings
    Serial.print("P1:"); Serial.print(pressureCalculation(pt1_analog));              // print pressure calculation in psi
    Serial.print(",P2:"); Serial.print(pressureCalculation(pt2_analog));              // print pressure calculation in psi
  }

  // Send volume since last signal
  /*
  -------------------------------------------------------------------
  VALVE ACTUATION
  -------------------------------------------------------------------
  */
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

  // Linearly Actuated Ball Valve 1
  if (input == "LA-BV 1:0\r\n") {
    digitalWrite(LABV1_pin, HIGH);
    is_LABV1_open = true;
  }
  if (input == "LA-BV 1:1\r\n") {
    digitalWrite(LABV1_pin, LOW);
    is_LABV1_open = false;
  }

  // Linearly Actuated Ball Valve 2
  if (input == "LA-BV 2:0\r\n") {
    digitalWrite(LABV2_pin, HIGH);
  }
  if (input == "LA-BV 2:1\r\n") {
    digitalWrite(LABV2_pin, LOW);
  }

  /*
  -------------------------------------------------------------------
  FLOW METER READING
  -------------------------------------------------------------------
  */
  if (pulseCount >= pulseReading) {   // determine if this should just send then time of pulse to be logged for post test calculation
    float volume = pulseCount * mlPerPulse;
    Serial.println(volume);
    pulseCount = 0;
  }

}