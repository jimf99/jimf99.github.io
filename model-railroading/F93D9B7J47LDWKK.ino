  
  // nrf24 - receive DCC preamble + 3 byte data   3 Jun 2017
    
    #include <SPI.h>
    #include "RF24.h"
  
   
   // use digital pins 6 and 5 for DCC out
    
    //Timer frequency is 2MHz for ( /8 prescale from 16MHz )
#define TIMER_SHORT 0x8D  // 58usec pulse length 
#define TIMER_LONG  0x1B  // 116usec pulse length 

unsigned char last_timer = TIMER_SHORT; // store last timer value
unsigned char flag = 0; // used for short or long pulse
unsigned char every_second_isr = 0;  // pulse up or down

// definitions for state machine
#define PREAMBLE 0
#define SEPERATOR 1
#define SENDBYTE  2

 RF24 radio(9,10);
 int inmsg[1];
 String theMessage;
 boolean stringComplete = false;
 const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
 

unsigned char state = PREAMBLE;
unsigned char preamble_count = 16;
unsigned char outbyte = 0;
unsigned char cbit = 0x80;
int b,c,d,e;
int locoSpeed = 0;
String  inString;
int locoAdr = 1;   // enter your loco number here
int a[12];

unsigned char  data = 0x00;
unsigned char  xdata = 0;

// buffer for command
struct Message {
  unsigned char data[7];
  unsigned char len;
} ;

#define MAXMSG 3
// for the time being, use only 2 messages - the idle msg, the loco Speed msg, function msg

struct Message msg[MAXMSG] = {
  { { 0xFF, 0, 0xFF, 0, 0, 0, 0}, 3},   // idle msg
  { { locoAdr, 0, 0, 0, 0, 0, 0}, 3}   // enter 3 for  3 byte and 4 for 4 byte DCC Command Station
};              

int msgIndex = 0;
int byteIndex = 0;

//Setup Timer2.
//Configures the 8-Bit Timer2 to generate an interrupt at the specified frequency.
//Returns the time load value which must be loaded into TCNT2 inside your ISR routine.

void SetupTimer2() {
  //Timer2 Settings: Timer Prescaler /8, mode 0
  //Timmer clock = 16MHz/8 = 2MHz oder 0,5usec
  TCCR2A = 0;
  TCCR2B = 0 << CS22 | 1 << CS21 | 0 << CS20;

  //Timer2 Overflow Interrupt Enable
  TIMSK2 = 1 << TOIE2;

  //load the timer for its first cycle
  TCNT2 = TIMER_SHORT;
}
//Timer2 overflow interrupt vector handler
ISR(TIMER2_OVF_vect) {
  //Capture the current timer value TCTN2. This is how much error we have
  //due to interrupt latency and the work in this function
  //Reload the timer and correct for latency.
  // for more info, see http://www.uchobby.com/index.php/2007/11/24/arduino-interrupts/
  unsigned char latency;
  // for every second interupt just toggle signal
  if (every_second_isr)  {
    PORTD = B01000000;  //use this instead of digitalWrite(6, 1); digitalWrite(5, 0);
    every_second_isr = 0;
    // set timer to last value
    latency = TCNT2;
    TCNT2 = latency + last_timer;
  }  else  {  // != every second interrupt, advance bit or state
    PORTD = B00100000;  //use this instead of digitalWrite(6, 0); digitalWrite(5, 1);
    every_second_isr = 1;
    switch (state)  {
      case PREAMBLE:
        flag = 1; // short pulse
        preamble_count--;
        if (preamble_count == 0)  {  // advance to next state
          state = SEPERATOR;
          // get next message
          msgIndex++;
          if (msgIndex >= MAXMSG)  {
            msgIndex = 0;
          }
          byteIndex = 0; //start msg with byte 0
        }
        break;
      case SEPERATOR:
        flag = 0; // long pulse
        // then advance to next state
        state = SENDBYTE;
        // goto next byte ...
        cbit = 0x80;  // send this bit next time first
        outbyte = msg[msgIndex].data[byteIndex];
        break;
      case SENDBYTE:
        if (outbyte & cbit)  {
          flag = 1;  // send short pulse
        }  else  {
          flag = 0;  // send long pulse
        }
        cbit = cbit >> 1;
        if (cbit == 0)  {  // last bit sent, is there a next byte?
          byteIndex++;
          if (byteIndex >= msg[msgIndex].len)  {
            // this was already the XOR byte then advance to preamble
            state = PREAMBLE;
            preamble_count = 16;
          }  else  {
            // send separtor and advance to next byte
            state = SEPERATOR ;
          }
        }
        break;
    }
    if (flag)  {  // if data==1 then short pulse
      latency = TCNT2;
      TCNT2 = latency + TIMER_SHORT;
      last_timer = TIMER_SHORT;
    }  else  {   // long pulse
      latency = TCNT2;
      TCNT2 = latency + TIMER_LONG;
      last_timer = TIMER_LONG;
    }
  }
}
 
    void setup(void)
    {
    Serial.begin(115200);
    //Set the pins for DCC "output".
    //pinMode(DCC_OUT_1, OUTPUT);  // 6 for the DCC Signal  
    //pinMode(DCC_OUT_2, OUTPUT);  // 5 for the inverse DCC Signal
    
    DDRD = B01100000;   //  register D5 fot digital pin 5, D6 fot digital pin 6 
    
    SetupTimer2();
    
    radio.begin();
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
    radio.startListening();

    Serial.println("Starting");
    }
    
   void amend_msg_byte3 (struct Message & x) { 
  x.data[1] = a[2];
  x.data[2] = a[3];
  x.data[3] = 0;
}

void amend_msg_byte4 (struct Message & x) { 
  x.data[1] = a[2];
  x.data[2] = a[3];
  x.data[3] = a[4];
}


 void loop(void){
 if (radio.available()){
    bool done = false;  
      done = radio.read(inmsg, 1); 
      char rc = inmsg[0];
      if (rc != 0){
        inString.concat(rc);
        }
      if (rc == '\0') {
      Serial.println(inString);
      string();
    }
  }
}
  

  void string(){
    radio.powerDown();
    unsigned long z = inString.length();
    int y = 0;
    for (int i = 0; i<=5; i++){
      a[i] = 0;
    }
String inChar;
String temp ="";
   for (int i = 0; i<=z; i++){
    inChar = inString.substring(i,i+1);
    if (inChar != "," && inChar != "D") {
      temp += inChar;
    }
    if (inChar == ",") {
      a[y] = (temp.toInt());
      y = y +1;
      temp = "";
    }
   }
   if (inString.length()>6 && inString.substring(0,1)== "D") {
     if (a[1] != locoAdr){ 
      inString = "";     
      radio.powerUp();
      return;
    }
    Serial.println(a[1]);
    Serial.println(a[2]);
    Serial.println(a[3]);
    Serial.println(a[4]);
    if (a[4] == 0 ) { 
    amend_msg_byte3(msg[1]);
    assemble_dcc_msg_3byte();}
    else
    {
    amend_msg_byte4(msg[1]);
    assemble_dcc_msg_4byte();}
   }
  //send_data2();
  inString = ""; 
  radio.powerUp();
    }

void assemble_dcc_msg_4byte() {
  noInterrupts();  
  msg[1].data[1] = a[2];
  msg[1].data[2] = a[3];
  msg[1].data[3] = a[4];
  interrupts();
}

void assemble_dcc_msg_3byte() {
  noInterrupts(); 
  msg[1].data[1] = a[2];
  msg[1].data[2] = a[3];
  interrupts();
}


void send_data2() {
  Serial.print("loco addr ");
  Serial.println(locoAdr);
  //Serial.print("loco speed ");
  //Serial.println(locoSpeed);
  Serial.print("msg[1].data[0] ");
  Serial.print(msg[1].data[0], BIN); Serial.print(" , "); Serial.print(msg[1].data[0], HEX); Serial.print("\n");
  Serial.print("msg[1].data[1] ");
  Serial.print(msg[1].data[1], BIN); Serial.print(" , "); Serial.print(msg[1].data[1], HEX); Serial.print("\n");
  Serial.print("msg[1].data[2] ");
  Serial.print(msg[1].data[2], BIN); Serial.print(" , "); Serial.print(msg[1].data[2], HEX); Serial.print("\n");
  Serial.print("msg[1].data[3] ");
  Serial.print(msg[1].data[3], BIN); Serial.print(" , "); Serial.print(msg[1].data[3], HEX); Serial.print("\n");

}


