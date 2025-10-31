#include <SoftwareSerial.h> // Include the SoftwareSerial library

// Initialize software serial on pins 2 (RX) and 3 (TX)
SoftwareSerial mySerial(2, 3);

// Define the size of the transmission buffer
const int TX_BUFFER_SIZE = 128;

// Create the transmission buffer
char txBuffer[TX_BUFFER_SIZE];

// Define head and tail indexes for the buffer management
volatile int txHead = 0; // Next write position
volatile int txTail = 0; // Next read position

/**
 * Adds a character to the transmission buffer
 *
 * @param c Character to be added to the buffer
 */
void addTxData(char c) {
    int nextHead = (txHead + 1) % TX_BUFFER_SIZE; // Calculate the next head position

    if (nextHead != txTail) { // Only add if there is space
        txBuffer[txHead] = c; // Add character to the buffer
        txHead = nextHead; // Move head to the next position
    }
}

/**
 * Prepares a request to be sent by adding specific characters to the buffer
 */
void sendRequest() {
    const char requestChars[] = {'T', 
                                 '0', 
                                 '1', 
                                 'F', 
                                 'B', 
                                 'L', 
                                 'R', 
                                 'S',
                                 'Z'
                                };
    
    for (char c : requestChars) {
                addTxData(c);
      			delay(60);
        }
}

/**
 * Parses the incoming response and extracts key-value pairs
 *
 * @param response The response string from the serial
 */
void parseResponse(String response) {
    // Split the response into key-value pairs
    int startIndex = 0;
    int equalsIndex = response.indexOf('=');

    while (equalsIndex != -1) {
        String key = response.substring(startIndex, equalsIndex);
        int endIndex = response.indexOf(',', equalsIndex); // Assuming values are comma-separated
        
        if (endIndex == -1) {
            endIndex = response.length(); // If it's the last pair
        }

        String value = response.substring(equalsIndex + 1, endIndex);
        if(key=="TIME") {
          Serial.println("");
        }
        Serial.print(key);
        Serial.print("=");
        Serial.println(value);

        // Prepare for next iteration
        startIndex = endIndex + 1;
        equalsIndex = response.indexOf('=', startIndex);
    }
}

void setup() {
    Serial.begin(9600); // Start hardware serial for debugging
    Serial.println("");

    mySerial.begin(2400); // Initialize software serial communication
}

void loop() {
    static unsigned long lastRequest = 0; // To track last request time

    if (millis() - lastRequest > 999) {
        sendRequest(); // Send the request
        lastRequest = millis(); // Update last request time
    }

    // If there is data to send in the buffer
    if (txHead != txTail) {
        mySerial.write(txBuffer[txTail]); // Send the character
        txTail = (txTail + 1) % TX_BUFFER_SIZE; // Move tail to the next position
    }

    // Check if there is available data from mySerial
    if (mySerial.available() > 0) {
        String response = mySerial.readStringUntil('\n'); // Read until a newline
        // parseResponse(response); // Parse the received response
      Serial.println(response);
      delay(100);
    }

    delay(10); // Small delay to allow other processes to run
}
