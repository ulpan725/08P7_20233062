// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// Configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // Sampling interval (unit: msec)
#define PULSE_DURATION 10 // Ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // Minimum distance to be measured (unit: mm)
#define _DIST_MAX 300.0   // Maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // Maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)      // Coefficient to convert duration to distance

unsigned long last_sampling_time;   // Unit: msec

// Function prototype for USS_measure
float USS_measure(int TRIG, int ECHO);

void setup() {
  // Initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // Sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);   // Sonar ECHO
  digitalWrite(PIN_TRIG, LOW);  // Turn-off Sonar 
  
  // Initialize serial port
  Serial.begin(57600);
}
void loop() {
  float distance;

  // Wait until the next sampling time.
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  distance = USS_measure(PIN_TRIG, PIN_ECHO); // Read distance

  if (distance >= 0 && distance <= 100) {
    // Distance is in the range 0mm-100mm, turn off the LED
    digitalWrite(PIN_LED, HIGH);
  } else if (distance > 300) {
    // Distance is over 300mm, turn off the LED
    digitalWrite(PIN_LED, HIGH);
  } else {
    // Calculate LED brightness based on distance
    int ledBrightness;

    if (distance >= 100 && distance <= 200) {
      // Gradually light up from 100mm to 200mm
      ledBrightness = map(distance, 100, 200, 255, 0); // Inverted mapping
    } else if (distance > 200 && distance <= 300) {
      // Gradually dim from 200mm to 300mm
      ledBrightness = map(distance, 200, 300, 0, 255);
    }

    analogWrite(PIN_LED, ledBrightness); // Set LED brightness
  }

  // Output the distance to the serial port
  Serial.print("Min:");        Serialcdf§                         .print(_DIST_MIN);
  Serial.print(",distance:");  Serial.print(distance);
  Serial.print(",Max:");       Serial.print(_DIST_MAX);
  Serial.println("");
  
  // Update the last sampling time
  last_sampling_time = millis();
}

// Get a §distance reading from USS. Return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  unsigned long pulseDuration = pulseIn(ECHO, HIGH, TIMEOUT); // Unit: microseconds

  // Calculate distance in millimeters
  float distance = (pulseDuration / 2.0) * (SND_VEL / 1000.0); // Convert microseconds to milliseconds

  // Check for invalid or out-of-range values
  if (distance <= 0.0 || distance > _DIST_MAX) {
    distance = _DIST_MAX + 10.0;
  }

  return distance;
}
