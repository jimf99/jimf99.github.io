/*
  Project: Theremin (UltraSonic Sensors)
  By: Jim F, Calgary AB (JimF99)
  Started: 21 July 2025
  Updated: 31 July 2025
  Inspired by: https://github.com/silveirago/Theremidi
  https://youtu.be/CWPtxGrBlU8?si=sW5UNRUgEtKDVIxn&t=618
  ===
  DIY Switches / Buttons
  https://www.instructables.com/Simple-Electronic-Button/
  https://www.instructables.com/How-to-Make-Big-Push-Button-That-Works/
  https://www.youtube.com/watch?app=desktop&v=xznKfiBmVZo&t=0s
*/

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x20,16,2);


// Global Variables
int spkr = 11;
int echo = 12;
int trigger = 13;
int ledRed=3;
int ledBlue=5;
int ledGreen=6;

int freq=0;
int dist=0;
int a0=0;
long ctr=0;
int octaveNum=0;
int midiNoteNum=0;

int scaleIndex=0;
String scaleName="------";
String noteName="--";

int midiChannel=0;
String pB="pB";

int minDist=37;
int maxDist=297;
int theOctave=0;

int theNote=0;
int zz=0;

static const char* noteNames[] = {
  "C", "C#", "D", "D#", "E", "F", "F#",  "G", "G#", "A", "A#", "B"
};

typedef struct
{
	uint8_t header;
	uint8_t byte1;
	uint8_t byte2;
	uint8_t byte3;
} midiEventPacket_t;


// Functions
int scaleNote(int scaleNum, int noteNum) {
  if(scaleNum==0) {
  int notes[]={ -1, 0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23, 24}; // 0: ionian
    scaleName="Jonian";
    return notes[noteNum];    
  }
  if(scaleNum==1) {
  int notes[]= { -1, 0, 2, 3, 5, 7, 8, 11, 12, 14, 15, 17, 19, 20, 23, 24}; // 4: min harm
    scaleName="MinHar";
    return notes[noteNum];    
  }
  if(scaleNum==2) {
  int notes[]= { -2, 0, 3, 5, 7, 10, 12, 15, 17, 19, 22, 24, 27, 29, 31, 34}; // 5: minor penta
    scaleName="MinPen";
    return notes[noteNum];    
  }
  if(scaleNum==3) {
    int notes[]={ -2, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28}; // 7: whltone
    scaleName="WhlTon";
    return notes[noteNum];    
  }
  return 0;
}

void splashScreen() {
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("ThereMIDI !");
  lcd.setCursor(2,1);
  lcd.print("Hello World!");
  delay(5000);
  lcd.clear();
}

int getDistance() {
  digitalWrite(trigger, LOW);
  delayMicroseconds(5);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  int duration = pulseIn(echo, HIGH);
  int dist = duration* 0.0343 / 2;
  return dist;
}

int getFreq(int val) {
  int freq=25+(val*7);
  if(val>255) {return 0;}
  return freq;
}

void putLeds(int rVal, int gVal, int bVal) {
  analogWrite(ledRed,rVal);
  analogWrite(ledGreen,gVal);
  analogWrite(ledBlue,bVal);
}

void setLeds(int val) {
  int led1=0;
  int led2=0;
  int led3=0;
  putLeds(0,0,0);
  if(val==1) {led1=255;}
  if(val==2) {led2=255;}
  if(val==3) {led3=255;}
  if(val==4) {led3=255;led1=255;}
  if(val==5) {led3=255;led2=255;}
  if(val==6) {led3=255;led2=255;led1=255;}
  putLeds(led1,led2,led3);
}

void doCmd(int cmdVal) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Button ");
  lcd.print(cmdVal);
  setLeds(cmdVal);
  delay(500);
}

void getButtons() {
    if(a0==512) {doCmd(1);}
    if(a0==682) {doCmd(2);}
    if(a0==767) {doCmd(3);}
    if(a0==818) {doCmd(4);}
    if(a0==853) {doCmd(5);}
    if(a0==877) {doCmd(6);}
}

void doSpkrTone(int freq) {
  if(freq==0) {
    noTone(spkr);
  }
  if(freq>0) {
    tone(spkr, freq, 1000);
  }
}

double midiNote2Freq(int noteNum) {
  return 440.0 * pow(2.0, (double)(midiNoteNum - 69) / 12.0);
}

void lcdUpdate() {
  lcd.setCursor(0,0);
  lcd.print(scaleName);
  lcd.print(" ");
  lcd.print(midiChannel+1);
  lcd.print(" ");
  lcd.print(pB);
  lcd.setCursor(0,1);
  lcd.print(noteName);
  lcd.print(" ");
  lcd.print(midiNoteNum);
  lcd.print(" ");
}

void lcdUpdate2() {
  lcd.setCursor(0,0);
  lcd.print(dist);
  lcd.print(" ");
  lcd.print(freq);
  lcd.print(" ");
  lcd.print(scaleIndex);
  lcd.print(" ");
  lcd.print(scaleName);
  lcd.print(" ");
  lcd.setCursor(0,1);
  lcd.print(noteName);
  lcd.print(" ");
  lcd.print(midiNoteNum);
  lcd.print(" ");
}

void readAnalogInputs() {
  a0=analogRead(A0);
  ctr=(ctr+1) % 10;
  if(ctr==1) {lcd.clear();putLeds(0,0,0);}
  if (a0 !=895) {lcd.clear();getButtons();}
}

const char* pitch_name(byte pitch) {
  return noteNames[pitch % 12];
}

int pitch_octave(byte pitch) {
  return (pitch / 12) - 1;
}

void noteOn(byte channel, byte pitch, byte velocity) {
  Serial.print("Note On: ");
  Serial.print(pitch_name(pitch));
  Serial.print(pitch_octave(pitch));
  Serial.print(", channel=");
  Serial.print(channel);
  Serial.print(", velocity=");
  Serial.println(velocity);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  Serial.print("Note Off: ");
  Serial.print(pitch_name(pitch));
  Serial.print(pitch_octave(pitch));
  Serial.print(", channel=");
  Serial.print(channel);
  Serial.print(", velocity=");
  Serial.println(velocity);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  // MidiUSB.sendMIDI(event);
}

void pitchBend(byte channel, int value) {
  byte lowValue = value & 0x7F;
  byte highValue = value >> 7;
  midiEventPacket_t pitchBend = {0x0E, 0xE0 | channel, lowValue, highValue};
  // MidiUSB.sendMIDI(pitchBend);
}


// Setup
void setup() {
  pinMode(ledRed,OUTPUT);
  pinMode(ledBlue,OUTPUT);
  pinMode(ledGreen,OUTPUT);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(spkr, OUTPUT);
  lcd.init();
  lcd.clear();         
  lcd.backlight();
  splashScreen();
  Serial.begin(115200);
}

// Main Loop
void loop() {
  octaveNum=4;
  dist=getDistance();
  theNote=(float(dist-minDist)/float(maxDist-minDist))*16;
  midiNoteNum=map(theNote,0,16,octaveNum*12,octaveNum*12+16);
  scaleNote(0,0);
  theOctave=(midiNoteNum/12);
  freq = int(midiNote2Freq(midiNoteNum));
  if(dist<minDist) {freq=0;}
  if(dist>maxDist) {freq=0;}
  noteName=pitch_name(theNote)+String(theOctave);
  readAnalogInputs();
  lcdUpdate();
  doSpkrTone(freq);
  int velocity=64;
  if(freq>0) {
    noteOn(midiChannel,midiNoteNum,velocity);
    noteOff(midiChannel,midiNoteNum,velocity);
  }
  if(1==2) {
    Serial.print(a0);
    Serial.print(",");
    Serial.println(freq);
  }
  delay(100);
}
