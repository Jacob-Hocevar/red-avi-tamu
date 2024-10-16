/*
-------------------------------------------------------------------
Water Test Stand Teensy
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

// BAUD rate 
const int BAUD = 115200;                   // serial com in bits per second     <-- USER INPUT

/*
-------------------------------------------------------------------
VALVE SETUP
-------------------------------------------------------------------
*/

// Valves
const int NCS1_pin = 0;           // <-- USER INPUT
const int NCS2_pin = 0;           // <-- USER INPUT



/*
-------------------------------------------------------------------
PRESSURE TRANSDUCER SET UP
-------------------------------------------------------------------
*/

// teensy pins to read signals
const int pt1_pin = 0;                    // <-- USER INPUT
const int pt2_pin = 0;                    // <-- USER INPUT

// callibration parameters for the pts
const float pt1_ref1 = 0;                       // known pressure low                <-- USER INPUT
const float pt1_ref2 = 0 + pt1_ref1;            // known pressure high               <-- USER INPUT
const float pt2_ref1 = 0;                       // known pressure low                <-- USER INPUT
const float pt2_ref2 = 0 + pt2_ref1;            // known pressure high               <-- USER INPUT

const int pt1_analog_ref1 = 0;         // analog at known pressure small    <-- USER INPUT
const int pt1_analog_ref2 = 0;         // analog at known pressure high     <-- USER INPUT
const int pt2_analog_ref1 = 0;         // analog at known pressure small    <-- USER INPUT
const int pt2_analog_ref2 = 0;         // analog at known pressure high     <-- USER INPUT

// analog and digital reading variables setup
int pt1_analog = 0;                        // analog reading from PT output signal
int pt2_analog = 0;                        // analog reading from PT output signal
float pt1_pressure = 0.0;                  // calculated pressure using calibration eq
float pt2_pressure = 0.0;                  // calculated pressure using calibration eq



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
  /*
  -------------------------------------------------------------------
  VALVE SET UP
  -------------------------------------------------------------------
  */

  pinMode(NCS1_pin, OUTPUT);    // sets the digital pin as output for controlling Valve 1 MOSFET
  pinMode(NCS2_pin, OUTPUT);    // sets the digital pin as output for controlling Valve 2 MOSFET

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

  /*
  -------------------------------------------------------------------
  VALVE ACTUATION
  -------------------------------------------------------------------
  */

  /*
  -------------------------------------------------------------------
  FLOW METER READING
  -------------------------------------------------------------------
  */

  // Send volume
  if (pulseCount >= pulseReading) { // determine if this should just send then time of pulse to be logged for post test calculation
    float volume = pulseCount * mlPerPulse;
    Serial.println(volume);
    pulseCount = 0;
  }

}