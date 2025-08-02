/*
  Project: ThereMIDI v3a
  Project by: Jim F, Calgary AB
  Date Started: 2 Aug 2025
  Date Updated: 2 Aug 2025
  
*/

#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Global Variables
int btn=0;
int a0=0;
int a1=0;
int spkr=3;
int sonar1Trig=2;
int sonar1Echo=4;
int sonar2Trig=5;
int sonar2Echo=6;
int dist1=0;
int dist2=0;
int freq=0;

// Ignore this first bit but don't delete or change it
// this bit is usually hidden inside 
// #import <NewPing.h>
// But I can't hide it in TinkerCad. 
class NewPing {
  public:
    NewPing(int TRIGGER_PIN, int ECHO_PIN, int MAX_DISTANCE ){
      trigPin=TRIGGER_PIN;
      echoPin=ECHO_PIN;
      maxDistance=MAX_DISTANCE;

      pinMode(trigPin,OUTPUT);
      pinMode(echoPin,INPUT);
    }

    int ping_cm(){
      // Clears the trigPin
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      // Sets the trigPin on HIGH state for 10 micro seconds
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      // Reads the echoPin, returns the sound wave travel time in microsecond
      long duration = pulseIn(echoPin, HIGH);
      // Calculating the distance
      int distance =  duration*0.034/2;
      // Checks out of range
      if (distance>maxDistance){
        distance=0;
      }
      return(distance);
    }

  private:
    int trigPin;
    int echoPin;
    int maxDistance;
};

NewPing sonar1(sonar1Trig,sonar1Echo,300);
NewPing sonar2(sonar2Trig,sonar2Echo,300);

// Functions
int checkButton(int val) {
  int rtn=0;
  if(val==0) {rtn=1;}
  if(val==511) {rtn=2;}
  if(val==682) {rtn=3;}
  if(val==767) {rtn=4;}
  if(val==818) {rtn=5;}
  if(val==852) {rtn=6;}
  if(val==877) {rtn=7;}
  if(val==895) {rtn=8;}
  return(rtn);
}

int checkKeypad(int val) {
  int rtn=0;
  if(val==479) {rtn=1;}
  if(val==407) {rtn=2;}
  if(val==313) {rtn=3;}
  if(val==184) {rtn=13;}
  if(val==668) {rtn=4;}
  if(val==638) {rtn=5;}
  if(val==604) {rtn=6;}
  if(val==562) {rtn=14;}
  if(val==759) {rtn=7;}
  if(val==743) {rtn=8;}
  if(val==726) {rtn=9;}
  if(val==705) {rtn=15;}
  if(val==813) {rtn=11;}
  if(val==803) {rtn=10;}
  if(val==793) {rtn=12;}
  if(val==781) {rtn=16;}
  return(rtn);
}

void doCmd(int cmd) {
  lcd.clear();
  lcd.setCursor(0,0);
  if(cmd>100 && cmd<200) {
    lcd.print(" B");
  }
  if(cmd>200 && cmd<300) {
    lcd.print(" K");
  }
  lcd.print(cmd);
  lcd.print(" ");
  delay(100);
  lcd.clear();
}

void updateDisplay() {
  lcd.setCursor(0,0);
  lcd.print(dist1);
  lcd.print("  ");
  lcd.print(dist2);
  lcd.print("  ");
  lcd.print(freq);
  lcd.print("  ");
}

void sendTone(int freq) {
  if(freq==0) {
    noTone(spkr);
  }
  if(freq>0) {
    tone(spkr,freq,1000);
  }
}

// Setup
void setup() {
  lcd.begin(16, 2);
  pinMode(spkr,OUTPUT);
  lcd.print("ThereMIDI");
  delay(2000);
  lcd.clear();
  Serial.begin(9600);
}

// Main Loop
void loop() {
  btn=0;
  a0=analogRead(A0);
  a1=analogRead(A1);
  if(btn==0) {btn=100+checkButton(a0);}
  if(btn==100) {btn=200+checkKeypad(a1);}
  if(btn==200) {btn=0;}
  if(btn>0) {doCmd(btn);}
  dist1=sonar1.ping_cm();
  dist2=sonar2.ping_cm();
  freq=map(dist2,33,300,55,2000);
  if(dist2<28 || dist2>290) {freq=0;}
  updateDisplay();
  sendTone(freq);
  delay(100);
}
