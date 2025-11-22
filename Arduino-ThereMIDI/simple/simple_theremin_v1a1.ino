/*
  Project: Simple Theremin v1a
  Programmed by: ChatGPT and Jim F, Calgary AB
  Date Started: 22 Nov 2025
  Date Updated: 22 Nov 2025
*/

#define TRIG_PIN1 2   // Pitch sensor trig pin for ultrasonic sensor 1
#define ECHO_PIN1 3   // Pitch sensor echo pin for ultrasonic sensor 1
#define TRIG_PIN2 4   // Volume sensor trig pin for ultrasonic sensor 2
#define ECHO_PIN2 5   // Volume sensor echo pin for ultrasonic sensor 2
#define BUZZER_PIN 9  // Piezo buzzer pin (PWM capable)
#define MAX_DISTANCE 200 // Maximum distance to measure in cm

// Optional smoothing of sensor readings
float pitchSmoothed = 0;          // Smoothed pitch distance
float volSmoothed = 0;            // Smoothed volume distance
float smoothingFactor = 0.2;      // Smoothing factor (0=no smoothing, 1=very smooth)

void setup() {
  // Set pin modes for ultrasonic sensors and buzzer
  pinMode(TRIG_PIN1, OUTPUT);     // Set pitch trig as output
  pinMode(ECHO_PIN1, INPUT);      // Set pitch echo as input
  pinMode(TRIG_PIN2, OUTPUT);     // Set volume trig as output
  pinMode(ECHO_PIN2, INPUT);      // Set volume echo as input
  pinMode(BUZZER_PIN, OUTPUT);    // Set buzzer pin as output
  
  Serial.begin(9600);             // Initialize serial communication at 9600 baud
  Serial.println("");             // Print empty line to separate data in serial monitor
}

// Function to read distance in cm from an HC-SR04 ultrasonic sensor
long readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);     // Ensure trig pin is low
  delayMicroseconds(2);           // Wait for 2 microseconds
  digitalWrite(trigPin, HIGH);    // Send 10us pulse to trigger sensor
  delayMicroseconds(10);          // Wait 10 microseconds
  digitalWrite(trigPin, LOW);     // Stop the pulse

  // Read the duration of the echo pulse (with 30ms timeout)
  long duration = pulseIn(echoPin, HIGH, 30000); 
  long distance = duration * 0.034 / 2; // Convert duration to distance in cm
  
  // Clamp distance: if 0 or exceeds max, set to MAX_DISTANCE
  if (distance == 0 || distance > MAX_DISTANCE) distance = MAX_DISTANCE;
  return distance;
}

// Function to generate PWM square wave tone with given frequency, amplitude, and duration
void playTonePWM(int freq, int amplitude, int durationMs) {
  long period = 1000000L / freq;                      // Full period in microseconds
  long pulse = (period / 2) * amplitude / 255;       // High time proportional to amplitude
  long start = millis();                              // Record start time

  // Generate PWM square wave for specified duration
  while (millis() - start < durationMs) {
    digitalWrite(BUZZER_PIN, HIGH);                  // Turn buzzer on
    delayMicroseconds(pulse);                        // Keep it high for pulse duration
    digitalWrite(BUZZER_PIN, LOW);                   // Turn buzzer off
    delayMicroseconds(period - pulse);               // Keep it low for remainder of period
  }
}

void loop() {
  // Read distances from sensors (inverted to make closer = higher value)
  long pitchDist = MAX_DISTANCE - readDistanceCM(TRIG_PIN1, ECHO_PIN1);
  long volDist = MAX_DISTANCE - readDistanceCM(TRIG_PIN2, ECHO_PIN2);

  // Smooth the readings to reduce jitter
  pitchSmoothed = pitchSmoothed + (pitchDist - pitchSmoothed) * smoothingFactor;
  volSmoothed = volSmoothed + (volDist - volSmoothed) * smoothingFactor;

  // Map pitch distance to frequency (Hz)
  int freq = map(pitchSmoothed, 2, 50, 2000, 200); // Smaller distance = higher pitch
  freq = constrain(freq, 100, 2000);               // Clamp frequency between 100Hz and 2000Hz

  // Map volume distance to amplitude (0-255)
  int amplitude = map(volSmoothed, 2, 50, 255, 0); // Smaller distance = louder
  amplitude = constrain(amplitude, 0, 255);        // Clamp amplitude within 0-255

  // Play tone for 20ms (fast updates for smooth continuous sound)
  playTonePWM(freq, amplitude, 20);

  // Output frequency and amplitude to serial monitor for debugging
  Serial.print(freq);
  Serial.print(",");
  Serial.print(amplitude);
  Serial.println("");
}
