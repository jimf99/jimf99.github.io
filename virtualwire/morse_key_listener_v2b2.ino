int ditLen=10;
int dahLen=30;
boolean Bin[8];
int key1Out=5;

#define SPEAKER_PIN 3

// The sound-producing function
void beep (unsigned char speakerPin, int frequencyInHertz, long timeInMilliseconds)
{ // http://web.media.mit.edu/~leah/LilyPad/07_sound_code.html
  int  x;
  long delayAmount = (long)(1000000 / frequencyInHertz);
  long loopTime = (long)((timeInMilliseconds * 1000) / (delayAmount * 2));
  pinMode(SPEAKER_PIN, OUTPUT);
  for (x = 0; x < loopTime; x++) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(delayAmount);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(delayAmount);
  }
  pinMode(SPEAKER_PIN, INPUT);
}

void setup() {
  pinMode(key1Out, OUTPUT);
}


void sendDit() {
  for(int i=0; i<ditLen*10; i++) {
    digitalWrite(key1Out,HIGH);
    delay(1);
  }
  digitalWrite(key1Out,LOW);
  beep(SPEAKER_PIN, 440,ditLen*10);
  delay(5);
}

void sendDah() {
  for(int i=0; i<dahLen*10; i++) {
    digitalWrite(key1Out,HIGH);
    delay(1);
  }
  digitalWrite(key1Out,LOW);
  beep(SPEAKER_PIN, 440,dahLen*10);
  delay(5);
}

void sendBin() {
  for(int i=0; i<=4; i++) {
    if(Bin[i]==0) {sendDit();}
    if(Bin[i]==1) {sendDah();}
  }
}

void loop()
{
  delay(50);
  Bin[0]=random(0,2);
  Bin[1]=random(0,2);
  Bin[2]=random(0,2);
  Bin[3]=random(0,2);
  Bin[4]=random(0,2);
  sendBin();
  delay(200);
  Bin[0]=random(0,2);
  Bin[1]=random(0,2);
  Bin[2]=random(0,2);
  Bin[3]=random(0,2);
  Bin[4]=random(0,2);
  sendBin();
}

