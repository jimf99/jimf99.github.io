#define POT  0
#define EN1  3
#define M1A  4
#define M1B  7
#define ENCA 5
#define ENCB 6

int pos = 0;
int MotorPower = 0;
boolean backwards = false;
int pulse=0;

void setup() {
  Serial.begin(115200);
  Serial.println("");
  pinMode(EN1, OUTPUT);
  pinMode(M1A, OUTPUT);
  pinMode(M1B, OUTPUT);
  pinMode (ENCA, INPUT_PULLUP);
  pinMode (ENCB, INPUT_PULLUP);
}

void loop(){
  pos = analogRead(POT);
  backwards=!(pos>=512);
  MotorPower=abs(512-pos)/2;
  MotorPower=map(MotorPower,0,255,0,240);
  digitalWrite(M1A, (backwards==true)?HIGH:LOW);
  digitalWrite(M1B, (backwards==true)?LOW:HIGH);
  analogWrite(EN1, MotorPower);
  int zz=digitalRead(5)*2+digitalRead(6);
  if(zz=3) {pulse+=(backwards==true)?-1:1;}
  Serial.print(backwards);
  Serial.print(",");
  Serial.print(MotorPower);
  Serial.print(",");
  Serial.print(pulse);
  Serial.println("");
  delay(10);
}

