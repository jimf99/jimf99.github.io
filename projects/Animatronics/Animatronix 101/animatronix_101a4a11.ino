#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

// --- Configuration & Pin Mapping ---
#define PIXEL_PIN      6
#define PIXEL_COUNT    12
#define SPEAKER_PIN    12
#define SERVO_L_PIN    9
#define SERVO_R_PIN    10
#define WAKE_PIN       2    
#define BUTTON_PIN     A0
#define MIRROR_SERVOS  true 

// --- Timing Constants ---
#define SLEEP_TIMEOUT  30000 
#define SPEECH_PAUSE   5000  
#define DEBOUNCE_MS    50    
#define HOLD_DELAY     500   
#define LCD_MSG_TIME   3000  

// --- Objects ---
LiquidCrystal_I2C lcd(0x20, 16, 2);
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
Servo servoL;
Servo servoR;

// --- Global Variables ---
unsigned long lastActivityTime = 0;
unsigned long lastButtonActionTime = 0;
unsigned long lastLcdUpdateTime = 0;
volatile bool wakeTriggered = false; 
bool isAsleep = false;
bool ledStates[6] = {false, false, false, false, false, false}; 
bool buttonReleased = true;

// --- Interrupt Service Routine ---
void wakeISR() {
  wakeTriggered = true;
}

// --- Button Logic ---
void handleButtons() {
  int rawValue = analogRead(BUTTON_PIN);

  // 1. Release check
  if (rawValue < 50) {
    buttonReleased = true;
    return;
  }

  // 2. Prevent rapid re-triggering
  if (!buttonReleased || (millis() - lastButtonActionTime < HOLD_DELAY)) return;

  // 3. Debounce confirm
  delay(DEBOUNCE_MS);
  int confirmVal = analogRead(BUTTON_PIN);
  if (abs(confirmVal - rawValue) > 30) return;

  // 4. Determine button index
  int btnIndex = -1;
  if (confirmVal >= 910 && confirmVal <= 960)      btnIndex = 0; 
  else if (confirmVal >= 820 && confirmVal <= 900) btnIndex = 1; 
  else if (confirmVal >= 750 && confirmVal <= 810) btnIndex = 2; 
  else if (confirmVal >= 710 && confirmVal <= 745) btnIndex = 3; 
  else if (confirmVal >= 650 && confirmVal <= 705) btnIndex = 4; 
  else if (confirmVal >= 50  && confirmVal <= 640) btnIndex = 5; 

  if (btnIndex != -1) {
    buttonReleased = false;
    lastButtonActionTime = millis();
    lastActivityTime = millis();
    
    // Toggle the state
    ledStates[btnIndex] = !ledStates[btnIndex];

    // Serial Print: Raw Voltage, Button #, and LED State
    Serial.print("RAW: ");     Serial.print(confirmVal);
    Serial.print(" | BTN: ");  Serial.print(btnIndex + 1);
    Serial.print(" | LED: ");  Serial.println(ledStates[btnIndex] ? "ON" : "OFF");

    // Update Hardware LED
    int pixelIdx = btnIndex + 6; 
    strip.setPixelColor(pixelIdx, ledStates[btnIndex] ? strip.Color(0, 0, 255) : strip.Color(0, 0, 0));
    strip.show();

    if (isAsleep) {
      wakeUp();
    } else {
      updateLcdStatus(btnIndex);
    }
  }
}

void updateLcdStatus(int btnIdx) {
  lastLcdUpdateTime = millis();
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Button: ");
  lcd.print(btnIdx + 1);
  lcd.print(ledStates[btnIdx] ? " ON " : " OFF");
}

void checkLcdTimeout() {
  if (!isAsleep && lastLcdUpdateTime > 0 && (millis() - lastLcdUpdateTime > LCD_MSG_TIME)) {
    lcd.setCursor(0, 1);
    lcd.print("SYSTEM: ACTIVE  ");
    lastLcdUpdateTime = 0; 
  }
}

// --- Smart Delay ---
bool smartDelay(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    handleButtons();
    checkLcdTimeout();
    if (wakeTriggered) return true;
    yield(); 
  }
  return false;
}

// --- Setup & Main Loop ---
void setup() {
  Serial.begin(9600);
  pinMode(WAKE_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(WAKE_PIN), wakeISR, FALLING);

  lcd.init();
  lcd.backlight();
  strip.begin();
  strip.setBrightness(100);
  strip.show();
  
  attachServos();
  writeServos(0); 
  
  for (int i = 5; i > 0; i--) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Booting");
    lcd.setCursor(0, 1);
    lcd.print("Ready in: "); lcd.print(i);
    setStripColor(50, 0, 0); 
    tone(SPEAKER_PIN, 440);
    smartDelay(500);
    noTone(SPEAKER_PIN);
    setStripColor(0, 0, 0);
    smartDelay(500);
  }
  wakeUp(); 
}

void loop() {
  handleButtons();
  checkLcdTimeout();

  if (wakeTriggered) {
    wakeTriggered = false;
    if (isAsleep) wakeUp();
    else lastActivityTime = millis();
  }

  if (!isAsleep) {
    simulateSpeech(random(5, 12));
    if (millis() - lastActivityTime > SLEEP_TIMEOUT) {
      goToSleep();
    } else {
      smartDelay(SPEECH_PAUSE);
    }
  } else {
    updateHeartbeat();
    smartDelay(20); 
  }
}

// --- Hardware Control ---
void attachServos() {
  if (!servoL.attached()) servoL.attach(SERVO_L_PIN);
  if (!servoR.attached()) servoR.attach(SERVO_R_PIN);
}

void detachServos() {
  servoL.detach();
  servoR.detach();
}

void writeServos(int angle) {
  if (!servoL.attached() || !servoR.attached()) attachServos();
  servoL.write(angle);
  servoR.write(MIRROR_SERVOS ? (180 - angle) : angle);
}

void simulateSpeech(int bursts) {
  for (int i = 0; i < bursts; i++) {
    if (wakeTriggered || isAsleep) return; 
    int openAngle = random(20, 50); 
    int speed = random(60, 140);    
    writeServos(openAngle);
    tone(SPEAKER_PIN, map(openAngle, 20, 50, 150, 350));
    if(smartDelay(speed)) break;
    writeServos(0); 
    noTone(SPEAKER_PIN);
    if(smartDelay(speed / 2)) break;
  }
}

void updateHeartbeat() {
  float b = (exp(sin(millis() / 2000.0 * PI)) - 0.36787944) * 108.0;
  for(int i = 0; i < 6; i++) strip.setPixelColor(i, strip.Color(b / 4, 0, 0));
  strip.show();
}

void goToSleep() {
  isAsleep = true;
  lcd.clear();
  lcd.print("SLEEP MODE");
  smartDelay(1000);
  lcd.noBacklight();
  lcd.noDisplay();
  detachServos(); 
}

void wakeUp() {
  isAsleep = false;
  lcd.display();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ANIMATRONIX 101a");
  lcd.setCursor(0, 1);
  lcd.print("SYSTEM: ACTIVE");
  attachServos();
  writeServos(0);
  for(int i = 0; i < 6; i++) strip.setPixelColor(i, strip.Color(0, 255, 0));
  strip.show();
  playStartupSound();
  lastActivityTime = millis();
}

void playStartupSound() {
  tone(SPEAKER_PIN, 440, 100);
  smartDelay(150);
  tone(SPEAKER_PIN, 880, 200);
}

void setStripColor(uint8_t r, uint8_t g, uint8_t b) {
  for(int i = 0; i < 6; i++) strip.setPixelColor(i, strip.Color(r, g, b));
  strip.show();
}
