/*
Project: 1D LED Pong
Sources:
  https://www.instructables.com/Arduino-LED-Pong-Game-a-Fun-Electronics-Project/
*/

void (*resetFunc)(void) = 0;

unsigned long previousMillis = 0;
unsigned long interval = 200;
unsigned long storeInterval = interval;
int score1 = 0;
int score2 = 0;
int soundA = 800;
int soundB = 1000;
int bounce = 0;

int startDir[] = { -1, 1 };
int LED[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5 };
const int button1 = A3;  
const int button2 = A0;  
int dir = 1;
int pos = 4;
int A = 0;
int B = 0;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 9; i++) {
    pinMode(LED[i], OUTPUT);
  }
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  dir = random(-1, 2);
  if (dir == 0) dir = 1;
}

int ledPos = 0;
int dly = 200;
bool doTest = false;

void test1() {
  digitalWrite(13 - ledPos, HIGH);
  delay(dly);
  digitalWrite(13 - ledPos, LOW);
  delay(dly);
  ledPos = (ledPos + 1) % 9;

  if (digitalRead(button1) == LOW) {
    digitalWrite(5, !digitalRead(5));
  }
  if (digitalRead(button2) == LOW) {
    digitalWrite(13, !digitalRead(13));
  }
}

void loop() {
  while (doTest) {
    test1();
  }

  unsigned long currentMillis = millis();
  int buttonState1 = digitalRead(button1);
  int buttonState2 = digitalRead(button2);

  Serial.print(buttonState1);
  Serial.print(",");
  Serial.print(buttonState2);
  Serial.print(",");
  Serial.print(dir);
  Serial.print(",");
  Serial.print(pos);
  Serial.print(",");
  Serial.print(score1);
  Serial.print(",");
  Serial.print(score2);
  Serial.println("," + String(interval));

  // Change direction when buttons are pressed
  if (buttonState1 == LOW && pos > 4 && A == 1) {
    dir = -1;
    A = 0;
    if (bounce == 1) {
      interval = storeInterval;
      bounce = 0;
    }
    if (interval > 40) interval -= 5;
    if (pos == 7) {
      bounce = 1;
      storeInterval = interval;
      interval = 60;
    }
  }

  if (buttonState2 == LOW && pos < 4 && B == 1) {
    dir = 1;
    B = 0;
    if (bounce == 1) {
      interval = storeInterval;
      bounce = 0;
    }
    if (interval > 40) interval -= 5;
    if (pos == 1) {
      bounce = 1;
      storeInterval = interval;
      interval = 60;
    }
  }

  // Reset button states
  if (buttonState1 == HIGH) A = 1;
  if (buttonState2 == HIGH) B = 1;

  if (currentMillis - previousMillis > interval) {
    tone(2, (dir == 1) ? soundB : soundA, 50);
    digitalWrite(LED[pos], LOW);

    pos += dir;
    if (pos < 0 || pos > 8) pos = constrain(pos, 0, 8);

    // Score handling
    if (pos == 0) {
      interval = storeInterval;
      for (int i = 0; i < 5; i++) {
        digitalWrite(LED[pos], HIGH);
        delay(50);
        tone(2, soundA, 50);
        digitalWrite(LED[pos], LOW);
        delay(50);
      }
      for (int i = 8; i >= 8 - score2; i--) {
        digitalWrite(LED[i], HIGH);
        delay(50);
      }
      delay(1000);
      for (int i = 8 - score2; i <= 8; i++) {
        digitalWrite(LED[i], LOW);
        delay(50);
      }
      digitalWrite(LED[pos], HIGH);
      delay(1000);
      dir = 1;
      score2++;
    }

    if (pos == 8) {
      interval = storeInterval;
      for (int i = 0; i < 5; i++) {
        digitalWrite(LED[pos], HIGH);
        delay(50);
        tone(2, soundB, 50);
        digitalWrite(LED[pos], LOW);
        delay(50);
      }
      for (int i = 0; i <= score1; i++) {
        digitalWrite(LED[i], HIGH);
        delay(50);
      }
      delay(1000);
      for (int i = score1; i >= 0; i--) {
        digitalWrite(LED[i], LOW);
        delay(50);
      }
      digitalWrite(LED[pos], HIGH);
      delay(1000);
      dir = -1;
      score1++;
    }

    // End of game check
    if (score1 == 9 || score2 == 9) {
      digitalWrite(LED[pos], LOW);
      for (int i = 0; i < 5; i++) {
        if (score1 == 9) {
          digitalWrite(LED[0], HIGH);
          delay(100);
          digitalWrite(LED[0], LOW);
          delay(100);
        }
        if (score2 == 9) {
          digitalWrite(LED[8], HIGH);
          delay(100);
          digitalWrite(LED[8], LOW);
          delay(100);
        }
        tone(2, 800, 100);
        tone(2, 900, 100);
        delay(200);
        noTone(2);
      }
      resetFunc();
    }

    digitalWrite(LED[pos], HIGH);
    previousMillis = currentMillis;
  }
  delay(20);
}
