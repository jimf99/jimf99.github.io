    
              
              //    ARDUINO TO SERVO FROM I2C
              //    This is part of an Instructable: http://www.instructables.com/member/InevitableCraftsLab/
              //    
              //    InevitableCraftsLab 2016
              //    
              //    Use it to send four analog values over 433MHz Tx/Rx and control four servos

#include <Wire.h>                                                                           //  INCLUDE THE WIRE LIBRARY
#include <Servo.h>                                                                          //  INCLUDE THE SERVO LIBRARY

Servo myservo;                                                                              //  ADD A SERVO OBJECT

      #if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)                     //  THIS IS FOR FEATHER BOARDS, FOR MORE INFO ON THAT
      #define Serial SERIAL_PORT_USBVIRTUAL                                                 //  FOR MORE INFO ON THAT 
      #endif                                                                                //  VISIT: https://learn.adafruit.com/adafruit-feather-m0-basic-proto/adapting-sketches-to-m0
      
void setup() 
{
  Wire.begin(8);                                                                            //  JOIN I2C WITH ADRESS 8
  Wire.onReceive(receiveEvent);                                                             //  REGISTER EVENT
  Serial.begin(9600);                                                                       //  BAUDE RATE FOR I2C 
  myservo.attach(10);                                                                       //  PIN FOR THE SERVO IS ON PIN 10
}

void loop() 
{
  delay(10);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)                                                              //  FUNCTION LISTENS IF DATA IS RECEIVED (REGISTERED AS EVENT, SEE ABOVE)
{
  while (1 < Wire.available()) 
{ 

}
    int x = Wire.read();                                                                      // RECEIVE BYTE AS INTEGER
//  Serial.println(x);                                                                      // UNCOMMENT TO PRINT THE VALUE ON THE SERIAL MONITOR
    myservo.write(x);                                                                         // MOVE THE SERVO TO POSITION X
} 






