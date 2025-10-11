// wac 21 june 2017   with 3 byte address;
// amended 28 speed steps see void speed_step()

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SPI.h"   
#include "RF24.h"

#include "Keypad.h"
const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns 
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {2,3,4,5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6,7,8}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
char key = keypad.getKey();

//                    addr, en,rw,rs,d4,d5,d6,d7
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7);  // Set the LCD I2C address

#define AN_SPEED   A0  // analog reading for Speed Poti


 RF24 radio(9,10);
 const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };


// variables for throttle / functions
int locoSpeed = 0;
int last_locoSpeed = 0;
int dir = 0;
int fl = 0;
int fun1 = 0;int fun2 = 0;int fun3 = 0;int fun4 = 0;int fun5 = 0;int fun6 = 0;int fun7 = 0;int fun8 = 0;  // functions

int teen = 0;
int last_locoAdr = 0;
int key_val;
int function = 0;
int last_function = 0;

unsigned char  data = 0, xdata;

int maxF =4;
int locoAdr = 9;   // this is the default address of the loco


// buffer for command
struct Message {
   unsigned char data[7];
   unsigned char len;
} ;

#define MAXMSG 3
// for the time being, use only 2 messages - the idle msg, the loco Speed msg, function msg

struct Message msg[MAXMSG] = { 
    { { 0xFF,     0, 0xFF, 0, 0, 0, 0}, 3},   // idle msg
    { { locoAdr, 0, 0,  0, 0, 0, 0}, 3}   // 3 byte address
  }; 

int la[20];// array to hold loco numbers
int sa[20];// array to hold speed values
int fda[20];// array to hold dir
int fla[20];// array to hold lights
int f1a[20];// array to hold fun1
int f2a[20];// array to hold fun2
int f3a[20];// array to hold fun3
int f4a[20];// array to hold fun4
int f5a[20];// array to hold fun5
int f6a[20];// array to hold fun6
int f7a[20];// array to hold fun7
int f8a[20];// array to hold fun8


void setup() {
   Serial.begin(115200);
   
    radio.begin();
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
    radio.stopListening();
    
  lcd.begin(20,4);         // initialize the lcd for 20 chars 4 lines, turn on backlight
  lcd.backlight();
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("Garden Train DCC");
  delay(2000);
  lcd.clear();
 

  //zero array
  for (int i = 0 ; i<=20 ; i++){
  la[i] = i; 
  sa[i] = 0;
  fda[i] = 0;
  fla[i] = 0;
  f1a[i] = 0;
  f2a[i] = 0;
  f3a[i] = 0;
  f4a[i] = 0;
  f5a[i] = 0;
  f6a[i] = 0;
  f7a[i] = 0;
  f8a[i] = 0;
 }
  read_locoSpeed();
  assemble_dcc_msg_speed();  
}

void amend_speed (struct Message & x) 
{ 
 x.data[0] = locoAdr;
 x.data[1] = 0x40;  // locoMsg with 28 speed steps 0x3f
}

void amend_group1 (struct Message & x) 
{ 
 x.data[0] = locoAdr;
 x.data[1] = 0x80;   // locoMsg with group one instruction 0x80
}



void loop(void) {
 
  if (read_locoSpeed())  {
       assemble_dcc_msg_speed();
       send_data_1();
       delay(10);
       send_data_3();
       send_data_4();
  }
  if (read_function())  {
       assemble_dcc_msg_group1(); 
       send_data_1();
       delay(10);
       send_data_3();
  }
  
}


boolean read_locoSpeed()  {
   
   boolean changed_l = false;
   data = 0;
   get_key();
  
 
   if (last_locoAdr != locoAdr && locoAdr > 0){
    changed_l = true;
    last_locoAdr = locoAdr;
    dir = fda[locoAdr];
    fl = fla[locoAdr];
    fun1 = f1a[locoAdr];
    fun2 = f2a[locoAdr];
    fun3 = f3a[locoAdr];
    fun4 = f4a[locoAdr];
    fun5 = f5a[locoAdr];
    fun6 = f6a[locoAdr];
    fun7 = f7a[locoAdr];
    fun8 = f8a[locoAdr];
    locoSpeed = sa[locoAdr];
    } 
   if (fda[locoAdr] != dir){
    changed_l = true;
    fda[locoAdr] = dir;
    }
   if (fda[locoAdr] == 0){
       data |= 0x40;
    }
    if (fda[locoAdr] == 1){
       data |= 0x60;
    } 
       
 if (locoAdr > 0){  
    locoSpeed = (28L * analogRead(AN_SPEED))/1023;
    if (last_locoSpeed < locoSpeed  || last_locoSpeed > locoSpeed ) {
      last_locoSpeed = locoSpeed;
      changed_l = true; 
      sa[locoAdr] = locoSpeed;
       }
       }
    //data |= sa[locoAdr];
    //locoSpeed = 28;
    speed_step();
    amend_speed(msg[1]);
    return changed_l;
}
   
boolean read_function()  {
   
   boolean changed_f = false; 
   data = 0;
   get_key();  
   amend_group1(msg[1]); 
   
 if (fla[locoAdr]  != fl){
    fla[locoAdr] = fl;
    changed_f = true;
   }
 if (fla[locoAdr] == 0) {
    data = 0x80;
   } //head lights off
   
 if (fla[locoAdr] == 1) {
    data = 0x90;
   } //head lights on
   
if (last_function != function){
    last_function = function;
    changed_f = true;
    //send_data();
   }

 
    if (f1a[locoAdr] != fun1){
    f1a[locoAdr] = fun1;
    changed_f = true;
    }
    if (f1a[locoAdr] == 0) {
    data |= 0;  // f1 off
    }
    if (f1a[locoAdr] == 1) {
    data |= 0x01; // f1 on
    }
    if (f2a[locoAdr] != fun2){
    f2a[locoAdr] = fun2;
    changed_f = true;
    }
    if (f2a[locoAdr] == 0) {
    data |= 0;
    }
    if (f2a[locoAdr] == 1) {
    data |= 0x02; // f2  on
    }
    if (f3a[locoAdr] != fun3){
    f3a[locoAdr] = fun3;
    changed_f = true;
    }
    if (f3a[locoAdr] == 0) {
    data |= 0;  
    }
    if (f3a[locoAdr] == 1) {
    data |= 0x04; // f3  on
    }
    if (f4a[locoAdr] != fun4){
    f4a[locoAdr] = fun4;
    changed_f = true;
    }
    if (f4a[locoAdr] == 0) {
    data |= 0;
    }
    if (f4a[locoAdr] == 1) {
    data |= 0x08; // f4  on
    }

 if (key_val >= 500 && key_val <= 800){
    data = 0xB0;                  // locoMsg with group two instruction
    if (f5a[locoAdr] != fun5){
    f5a[locoAdr] = fun5;
    changed_f = true;
    }
    if (f5a[locoAdr] == 0) {
    data |= 0;
    }
    if (f5a[locoAdr] == 1) {
    data |= 0x01; // f5  on
    }
    if (f6a[locoAdr] != fun6){
    f6a[locoAdr] = fun6;
    changed_f = true;
    }
    if (f6a[locoAdr] == 0) {
    data |= 0;
    }
    if (f6a[locoAdr] == 1) {
    data |= 0x02; // f6  on
    }
    if (f7a[locoAdr] != fun7){
    f7a[locoAdr] = fun7;
    changed_f = true;
    }
    if (f7a[locoAdr] == 0) {
    data |= 0;
    }
    if (f7a[locoAdr] == 1) {
    data |= 0x04; // f7  on
    }
    if (f8a[locoAdr] != fun8){
    f8a[locoAdr] = fun8;
    changed_f = true;
    }
    if (f8a[locoAdr] == 0) {
    data |= 0;
    }
    if (f8a[locoAdr] == 1) {
    data |= 0x08; // f8  on
    } 
  }
    return changed_f;
}


void assemble_dcc_msg_speed() {
   // add XOR byte 
   xdata = (msg[1].data[0] ^ data);
   msg[1].data[1] = data;
   msg[1].data[2] = xdata;
}

void assemble_dcc_msg_group1() {
   // add XOR byte 
   xdata = (msg[1].data[0] ^ data); 
   msg[1].data[1] = data;
   msg[1].data[2] = xdata; 
}


void get_key(){
  char key = keypad.getKey();
  
   if (key != NO_KEY){
    //Serial.println(key);
    key_val = key - '0';
    locoAdr = constrain(locoAdr, 1, 9);
    if (key_val == -13){
      function ^= 1;
    } 

 if (key_val >= 1 && key_val <= 9 && function == 0){
      locoAdr = key_val;
    }

 if (key_val == -6){
     dir ^= 1;
    }
 if (key_val == 0){
     fl ^= 1;
    }

 if (key_val >= 1 && key_val <= 9 && function == 1){
      key_val = key_val * 100;
    }
 if (key_val == 100 && function == 1){
     fun1 ^= 1;
    }
 if (key_val == 200 && function == 1){
     fun2 ^= 1;
    }
 if (key_val == 300 && function == 1){
     fun3 ^= 1;
    }
 if (key_val == 400 && function == 1){
     fun4 ^= 1;
    }
 if (key_val == 500 && function == 1){
     fun5 ^= 1;
    }
 if (key_val == 600 && function == 1){
     fun6 ^= 1;
    }
 if (key_val == 700 && function == 1){
     fun7 ^= 1;
    }
 if (key_val == 800 && function == 1){
     fun8 ^= 1;
    }
   }
}

void send_data_1(){
  
  String s1 = String(msg[1].data[0], DEC) + ",";
  String s2 = String(msg[1].data[1], DEC) + ",";
  String s3 = String(msg[1].data[2], DEC) + ",";
  String s4 = String(msg[1].data[3], DEC) + ",";
  String theMessage = ("DD," + s1 + s2 + s3 + s4 + '\0');
   Serial.println(theMessage);
   int messageSize = theMessage.length();
  for (int i = 0; i <= messageSize; i++) {
    int charToSend[1];
    charToSend[0] = theMessage.charAt(i);
    radio.write(charToSend,1);
  }
}
void send_data_2(){
  lcd.setCursor(0, 0);
  lcd.print("Loco ");
  lcd.print(locoAdr, DEC);
  lcd.print(" Dir ");
  lcd.print(fda[locoAdr], DEC);
  lcd.print(" Light ");
  lcd.print(fla[locoAdr], DEC);
  lcd.setCursor(0, 1);
  lcd.print("Speed ");
  lcd.print(sa[locoAdr]);  
  lcd.setCursor(0, 2);
  lcd.print("1 2 3 4 5 6 7 8");
  lcd.setCursor(0, 3);
  lcd.print(f1a[locoAdr]);
  lcd.print(" ");
  lcd.print(f2a[locoAdr]);
  lcd.print(" ");
  lcd.print(f3a[locoAdr]);
  lcd.print(" ");
  lcd.print(f4a[locoAdr]);
  lcd.print(" ");
  lcd.print(f5a[locoAdr]);
  lcd.print(" ");
  lcd.print(f6a[locoAdr]);
  lcd.print(" ");
  lcd.print(f7a[locoAdr]);
  lcd.print(" ");
  lcd.print(f8a[locoAdr]);
  lcd.print(" ");
  if (function == 1){
   lcd.print("*"); 
  }
  if (function == 0){
   lcd.print("-"); 
  }
  
}

void send_data_3(){
  String s5 = "Loco " + String(locoAdr) + " Dir " + String(fda[locoAdr]) + " Light " + String(fla[locoAdr]);
  lcd.setCursor(0, 0);
  lcd.print(s5);
  
  String s6 = "Speed " + String(sa[locoAdr])+" ";
  lcd.setCursor(0, 1);
  lcd.print(s6);

  String s7 = "Fun 1 2 3 4 5 6 7 8";
  lcd.setCursor(0, 2);
  lcd.print(s7);

  lcd.setCursor(0, 3);
  if (function == 1){
   lcd.print("  # "); 
  }
  if (function == 0){
   lcd.print("  - "); 
  }
  String s8 = String(f1a[locoAdr])+" "+String(f2a[locoAdr])+" "+String(f3a[locoAdr])+" "+String(f4a[locoAdr])+" "+String(f5a[locoAdr])+" "+String(f6a[locoAdr])+" "+String(f7a[locoAdr])+" "+String(f8a[locoAdr]); 
  lcd.print(s8);
  
  
  
}

void send_data_4(){
   Serial.print("msg[1].data[0] ");
   Serial.print(msg[1].data[0], BIN);Serial.print(" , ");Serial.print(msg[1].data[0], HEX);Serial.print("\n");
   Serial.print("msg[1].data[1] ");
   Serial.print(msg[1].data[1], BIN);Serial.print(" , ");Serial.print(msg[1].data[1], HEX);Serial.print("\n");
   Serial.print("msg[1].data[2] ");
   Serial.print(msg[1].data[2], BIN); Serial.print(" , ");Serial.print(msg[1].data[2], HEX);Serial.print("\n");  
}

void speed_step(){
  switch (locoSpeed){
  case 1: 
    data |= 0x02;
    break;
  case 2: 
    data |= 0x12;
    break;
  case 3: 
    data |= 0x03;
    break;
  case 4: 
    data |= 0x13;
    break;
  case 5: 
    data |= 0x04;
    break;
  case 6: 
    data |= 0x14;
    break;
  case 7: 
    data |= 0x05;
    break;
  case 8: 
    data |= 0x15;
    break;
  case 9: 
    data |= 0x06;
    break;
  case 10: 
    data |= 0x16;
    break;
  case 11: 
    data |= 0x07;
    break;
  case 12: 
    data |= 0x17;
    break;
  case 13: 
    data |= 0x08;
    break;
  case 14: 
    data |= 0x18;
    break;
  case 15: 
    data |= 0x09;
    break;
  case 16: 
    data |= 0x19;
    break;
  case 17: 
    data |= 0x0A;
    break;
  case 18: 
    data |= 0x1A;
    break;
  case 19: 
    data |= 0x0B;
    break;
  case 20: 
    data |= 0x1B;
    break;
  case 21: 
    data |= 0x0C;
    break;
  case 22: 
    data |= 0x1C;
    break;
  case 23: 
    data |= 0x0D;
    break;
  case 24: 
    data |= 0x1D;
    break;
  case 25: 
    data |= 0x0E;
    break;
  case 26: 
    data |= 0x1E;
    break;
  case 27: 
    data |= 0x0F;
    break;
  case 28: 
    data |= 0x1F;
    break;
  }
}

