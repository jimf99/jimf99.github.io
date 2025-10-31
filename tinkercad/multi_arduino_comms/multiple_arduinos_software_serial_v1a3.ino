#include <SoftwareSerial.h>

/*
  Project: Ardunio Uno - 2 Wheeled Balancing Bot
*/

// Global Variables
int val = 0;
int front = 0;
int back = 0;
int left = 0;
int right = 0;

float kp = 0.0f;
float kd = 0.0f;
float kp_speed = 0.0f;
float ki_speed = 0.0f;
float angle = 0.0f;
float kp_turn = 0.0f;
float kd_turn = 0.0f;
float TT = 0.0f;
float angle0 = 0.0f;

unsigned long telemHeartBeat=0;


// Define software serial pins (e.g., RX on pin 10, TX on pin 11)
SoftwareSerial mySerial(2,3); // RX, TX

void setup() {
    Serial.begin(9600);       // Initialize standard serial
    mySerial.begin(2400);    // Initialize software serial
}

void loop() {  
    // Check if data is available on the main serial port
    if (Serial.available()) {
        char val = Serial.read(); // Read value from serial port
        handleCommand(val); // Process the command

        if (isValidCommand(val)) {
            TT = angle0;
        } else {
            TT = val;
            angle0 = TT;
        }
    }

    // Check if data is available on the software serial port
    if (mySerial.available()) {
        char val = mySerial.read(); // Read value from software serial port
        handleCommand(val); // Process the command

        if (isValidCommand(val)) {
            TT = angle0;
        } else {
            TT = val;
            angle0 = TT;
        }
    }

    // Check for telemetry heartbeat
    if (milsec(telemHeartBeat) > 50) {
        telemetry();
        telemHeartBeat = milsec(0);
    }

    delay(10);
}

/// Functions

unsigned long milsec(unsigned long prevMs) {
  return millis()-prevMs;
}


void handleCommand(int command) {
    switch (command) {
        case 'F': front = 250; break; // Move front
        case 'B': back = -250; break; // Move back
        case 'L': left = 1; break;    // Turn left
        case 'R': right = 1; break;   // Turn right
        case 'S': stopMovement();      // Stop
        case 'Q': break; // sendMsg("angle=" + String(angle)); break; // Send angle
        case 'P': updateKp(0.5); break; // Increase Kp
        case 'O': updateKp(-0.5); break; // Decrease Kp
        case 'I': updateKd(0.02); break; // Increase Kd
        case 'U': updateKd(-0.02); break; // Decrease Kd
        case 'Y': updateKpSpeed(0.05); break; // Increase Kp speed
        case 'T': updateKpSpeed(-0.05); break; // Decrease Kp speed
        case 'G': updateKiSpeed(0.01); break; // Increase Ki speed
        case 'H': updateKiSpeed(-0.01); break; // Decrease Ki speed
        case 'J': updateKpTurn(0.4); break; // Increase Kp turn
        case 'K': updateKpTurn(-0.4); break; // Decrease Kp turn
        case 'N': updateKdTurn(0.01); break; // Increase Kd turn
        case 'M': updateKdTurn(-0.01); break; // Decrease Kd turn
    }
}

void stopMovement() {
    front = 0;
    back = 0;
    left = 0;
    right = 0;
}

bool isValidCommand(int command) {
    switch (command) {
        // List of valid commands that return true
        case 'F': // Forward command
        case 'B': // Backward command
        case 'L': // Left turn command
        case 'R': // Right turn command
        case 'S': // Stop command
        case 'Q': // Send angle command (commented out in handleCommand)
        case 'P': // Increase Kp command
        case 'O': // Decrease Kp command
        case 'I': // Increase Kd command
        case 'U': // Decrease Kd command
        case 'Y': // Increase Kp speed command
        case 'T': // Decrease Kp speed command
        case 'G': // Increase Ki speed command
        case 'H': // Decrease Ki speed command
        case 'J': // Increase Kp turn command
        case 'K': // Decrease Kp turn command
        case 'N': // Increase Kd turn command
        case 'M': // Decrease Kd turn command
            return true; // Return true if the command is one of the valid options

        default: // If none of the above cases match
            return false; // Return false for invalid commands
    }
}
// Function to update the proportional gain for the PID controller
void updateKp(float value) {
    kp += value; // Increments the proportional gain (kp) by the specified value
}

// Function to update the derivative gain for the PID controller
void updateKd(float value) {
    kd += value; // Increments the derivative gain (kd) by the specified value
}

// Function to update the proportional gain for speed control
void updateKpSpeed(float value) {
    kp_speed += value; // Increments the speed proportional gain (kp_speed) by the specified value
}

// Function to update the integral gain for speed control
void updateKiSpeed(float value) {
    ki_speed += value; // Increments the speed integral gain (ki_speed) by the specified value
}

// Function to update the proportional gain for turn control
void updateKpTurn(float value) {
    kp_turn += value; // Increments the turn proportional gain (kp_turn) by the specified value
}

// Function to update the derivative gain for turn control
void updateKdTurn(float value) {
    kd_turn += value; // Increments the turn derivative gain (kd_turn) by the specified value
}

// Sends a message via serial communication
void sendMsg(const String& message) {
    Serial.println(message); // Output the message to the primary serial interface for debugging
    mySerial.println(message); // Send the message to a secondary serial interface (e.g., Bluetooth)
    delay(100); // Pause for 100 milliseconds to ensure message is sent properly
}


// Collects telemetry data and sends it as a formatted message
void telemetry() {
    String telemetryMsg = "Telemetry: "; 
    
    telemetryMsg += "front=" + String(front) + ", "; 
    telemetryMsg += "back=" + String(back) + ", ";   
    telemetryMsg += "left=" + String(left) + ", ";   
    telemetryMsg += "right=" + String(right) + ", "; 
    telemetryMsg += "kp=" + String(kp) + ", ";       
    telemetryMsg += "kd=" + String(kd) + ", ";       
    telemetryMsg += "kp_speed=" + String(kp_speed) + ", "; 
    telemetryMsg += "ki_speed=" + String(ki_speed) + ", "; 
    telemetryMsg += "angle=" + String(angle) + ", "; 
    telemetryMsg += "kp_turn=" + String(kp_turn) + ", "; 
    telemetryMsg += "kd_turn=" + String(kd_turn) + ", "; 
    telemetryMsg += "TT=" + String(TT) + ", ";      
    telemetryMsg += "angle0=" + String(angle0);      

    sendMsg(telemetryMsg); 
}