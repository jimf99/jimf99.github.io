#include <SoftwareSerial.h>

// Define the pins for the software serial
#define RX_PIN 2 // Use GPIO 16 for RX for ESP32
#define TX_PIN 3 // Use GPIO 17 for TX for ESP32
SoftwareSerial mySerial(RX_PIN, TX_PIN); // RX, TX

const int TX_BUFFER_SIZE = 128;
char txBuffer[TX_BUFFER_SIZE]; 
volatile int txHead = 0;       
volatile int txTail = 0;       

// Function to add a single character to the transmission buffer
void addTxData(char c) {
    int nextHead = (txHead + 1) % TX_BUFFER_SIZE;

    if (nextHead != txTail) {
        txBuffer[txHead] = c;  
        txHead = nextHead;     
    } else {
        Serial.println("Buffer full, could not add data");
    }
}

// Function to add a String to the transmission buffer
void addTxDataString(const String& data) {
    for (int i = 0; i < data.length(); i++) {
        addTxData(data.charAt(i));
    }
    Serial.print(data);
}

// Function to handle incoming commands
String handleCommand(String cmd) {
    String newLine = "\r\n";
    String response;
  response = "ERR=" + String(random(0, 32767) + 32767) + newLine;
  if (cmd=="T") {
            response = newLine + "TIME=" + String(millis() / 1000) + newLine;
  }
  if (cmd=="0") {
            response = "A0=" + String(analogRead(A0)) + newLine;
  }
  if (cmd=="1") {
            response = "A1=" + String(analogRead(A1)) + newLine;
  }
  if (cmd=="F") {
            response = "FORWARD=" + String(1) + newLine;
  }
  if (cmd=="B") {
            response = "BACK=" + String(1) + newLine;
  }
  if (cmd=="L") {
            response = "LEFT=" + String(1) + newLine;
  }
  if (cmd=="R") {  
            response = "RIGHT=" + String(1) + newLine;
  }
  if (cmd=="S") {
          response = "STOP=" + String(1) + newLine;
  }

    return response;
}

// Arduino setup function
void setup() {
    Serial.begin(9600);      
    Serial.println("");      
    mySerial.begin(2400);    
}

// Main loop function
void loop() {
    String newLine = "\r\n";

    // Check if data is available on the software serial port
    while (mySerial.available() > 0) {
        char incomingByte = mySerial.read();

        // Call the handleCommand function to get the response
        String response = handleCommand(""+String(incomingByte));

        // Add the response to the transmission buffer if it's not empty
        if (response.length() > 0) {
            addTxDataString(response);
        }
    }

    // Transmit data if buffer has data to send
    if (txHead != txTail) {
        mySerial.write(txBuffer[txTail]); 
        txTail = (txTail + 1) % TX_BUFFER_SIZE; 
    }

    delay(10); // Small delay for readability and processing
}
