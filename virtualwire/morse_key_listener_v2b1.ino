/*
  Project : Morse Key Listener v2b
  Project by: Jim F, Calgary AB
  Date Started : October 9 , 2025
  Date Updated : October 9, 2025
*/

#include <avr/pgmspace.h>

// Global Variables
int ctr=0;
unsigned long startMs=0;
int endMs=0;
int state=0;
unsigned long d2Len;
int key1Inp=2;
int led1Out=3;
int spkrOut=5;
int ditLen=300;
int dahLen=600;
unsigned long lenMs=0;
unsigned long keyUpMs=0;
int ditDas[9];

// Control characters
#define LTRS_SHIFT_BAUDOT 0x1F // Binary 11111
#define FIGS_SHIFT_BAUDOT 0x1B // Binary 11011
#define CR_BAUDOT         0x08 // Binary 01000
#define LF_BAUDOT         0x02 // Binary 00010

// LTRS mode lookup table (0-31)
const char LTRS_TABLE[] PROGMEM = {
  ' ', '_', 'E', LF_BAUDOT, 'A', ' ', 'I', 'U', '\r', 'O', ' ', 'H', 'N', 'J', 'F', 'C',
  'K', 'T', 'Z', 'L', 'W', 'D', 'R', 'Y', 'G', 'M', 'P', 'X', 'V', ' ', 'B', 'S'
};

// FIGS mode lookup table (0-31)
const char FIGS_TABLE[] PROGMEM = {
  ' ', ' ', '3', '\n', '-', ' ', '8', '7', '\r', '9', ' ', '#', ',', '.', ':', '5',
  '(', '5', '+', ')', '2', '$', '4', '6', '&', '.', '0', '/', ';', ' ', '?', '\''
};

bool baudotMode = true; // true = LTRS, false = FIGS


// Function to decode 5-bit Baudot to ASCII
char decodeBaudot(uint8_t baudotByte) {
  if (baudotByte < 32) {
    if (baudotMode) {
      return (char)pgm_read_byte_near(LTRS_TABLE + baudotByte);
    } else {
      return (char)pgm_read_byte_near(FIGS_TABLE + baudotByte);
    }
  }
  return 0; // Invalid Baudot code
}

// Setup
void setup() {
  Serial.begin(9600);
  Serial.println("\n=== BEGIN ===\n");
  pinMode(key1Inp, INPUT);
  pinMode(led1Out, OUTPUT);
}

// Main Loop
void loop() {
 int d2=digitalRead(key1Inp);
 if(d2==1 && startMs==0) {startMs=millis();}
 if(d2==0 && keyUpMs==0) {keyUpMs=millis();}
 digitalWrite(led1Out,d2);
  if(d2==1) {tone(spkrOut,440);}
  if(d2==0) {noTone(spkrOut);}
 if(d2 != state) {
   lenMs=millis()-startMs;
   if(lenMs>0) {
     if(lenMs>10 && lenMs<=ditLen) {
       ctr=(ctr % 5);
       ctr++;
       ditDas[ctr]=0;
     }
     if(lenMs>ditLen && lenMs<=dahLen) {
       ctr=(ctr % 5);
       ctr++;
       ditDas[ctr]=1;
     }
   }
   if(ctr==5) {
     int val=0;
     int pwr=1;
     for(int i=1; i<=5; i++) {
       if(ditDas[i]==0) {Serial.print("0 ");}
       if(ditDas[i]==1) {Serial.print("1 ");val=val+pwr;}
       pwr=pwr*2;
     }
     Serial.print(val);
     Serial.print(" ");
     Serial.print(char(decodeBaudot(val)));
     Serial.println("");
     ctr=0;
   }
   startMs=0; 
 }
 if(d2!=state) {state=0;keyUpMs=0;}
 state=d2;
 delay(1);
}