/*
*/

// Global Variables
int ditLen=100;
int dahLen=3*ditLen;
boolean Bin[8];
int key1Out=5;
int toneFreq=110*6;
int spkrPin=3;

// Functions
void sendDit() {
  tone(spkrPin, toneFreq);
  for(int i=0; i<ditLen; i++) {
    digitalWrite(key1Out,HIGH);
    delay(1);
  }
  digitalWrite(key1Out,LOW);
  noTone(spkrPin);
  delay(5);
}

void sendDah() {
  tone(spkrPin, toneFreq);
  for(int i=0; i<dahLen; i++) {
    digitalWrite(key1Out,HIGH);
    delay(1);
  }
  digitalWrite(key1Out,LOW);
  noTone(spkrPin);
  delay(5);
}

void sendBin() {
  for(int i=0; i<=4; i++) {
    sendDitOrDah(Bin[i]);
  }
}

void sendDitOrDah(int ditDah) {
  if(ditDah==0) {sendDit();}
  if(ditDah==1) {sendDah();}
}

void send5(int b1, int b2, int b3, int b4, int b5) {
  sendDitOrDah(b1);
  sendDitOrDah(b2);
  sendDitOrDah(b3);
  sendDitOrDah(b4);
  sendDitOrDah(b5);
  int val=0;
  val=b1+b2*2+b3*4+b4*8+b5*16;
  Serial.print(b1);
  Serial.print(b2);
  Serial.print(b3);
  Serial.print(b4);
  Serial.print(b5);
  Serial.print(" ");
  Serial.print(val);
  Serial.println("");
}

// Setup
void setup() {
  pinMode(key1Out, OUTPUT);
  pinMode(spkrPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("");
}

// Main Loop
void loop() {
  delay(50);
  int d1=random(0,2);
  int d2=random(0,2);
  int d3=random(0,2);
  int d4=random(0,2);
  int d5=random(0,2);
  send5(d1,d2,d3,d4,d5);
}

