    
              
              //    ARDUINO TO 433 TX
              //    This is part of an Instructable: http://www.instructables.com/member/InevitableCraftsLab/
              //    
              //    InevitableCraftsLab 2016
              //    
              //    Use it to send four analog values over 433MHz Tx/Rx and control four servos

#include <VirtualWire.h>                                                                     //  INCLUDE THE VIRTUALWIRE LIBRARY
#include <Wire.h>                                                                            //  INCLUDE THE WIRE LIBRARY

int Sensor1Data;                                                                             //  VARIABLE WHERE THE ANALOG VALUE OF POT 1 GOT STORED BY THE TX
int Sensor2Data;                                                                             //  VARIABLE WHERE THE ANALOG VALUE OF POT 2 GOT STORED BY THE TX
int Sensor3Data;                                                                             //  VARIABLE WHERE THE ANALOG VALUE OF POT 3 GOT STORED BY THE TX
int Sensor4Data;                                                                             //  VARIABLE WHERE THE ANALOG VALUE OF POT 4 GOT STORED BY THE TX

 
char StringReceived[22]; 
 
void setup() {
  
    Wire.begin();                                                                             //  START I2C (WIRE.h)
    vw_setup(6000);                                                                           //  BAUDERATE FOR VIRTUALWIRE
    vw_set_rx_pin(11);                                                                        //  DEFINE PIN FOR VIRTUALWIRE
    vw_rx_start();                                                                            //  START VIRTUALWIRE                                           
} 
  
void loop(){
  
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    if (vw_get_message(buf, &buflen)) {                                                       // GET THE DATA

 int i;
        for (i = 0; i < buflen; i++)  {                                                       // CHECKSUM OK ? GET MESSAGE           
          StringReceived[i] = char(buf[i]);                                                   // FILL THE ARRAY 
 } 
      sscanf(StringReceived, "%d,%d,%d,%d,%d,%d",&Sensor1Data, &Sensor2Data,&SensorData3,&SensorData4); // Converts a string to an array
  
      Sensor1Data = map(Sensor1Data, 0, 1023, 26, 160);                                                 //  MAPS THE 8BIT SERVODATA TO SERVOMIN/MX
      Sensor2Data = map(Sensor2Data, 0, 1023, 26, 160);                                                 //  MAPS THE 8BIT SERVODATA TO SERVOMIN/MX
      Sensor3Data = map(Sensor3Data, 0, 1023, 26, 160);                                                 //  MAPS THE 8BIT SERVODATA TO SERVOMIN/MX
      Sensor4Data = map(Sensor4Data, 0, 1023, 26, 160);                                                 //  MAPS THE 8BIT SERVODATA TO SERVOMIN/MX
      
      Wire.beginTransmission(8);                                                                        //  OPENS AN I2C ON PIN 8
      Wire.write(Sensor1Data);                                                                          //  SEND POT 1 VALUE TO I2C DEVICE 8
      Wire.endTransmission();                                                                           //  END TRANSMISSION
       
    }
  
 memset( StringReceived, 0, sizeof( StringReceived));                                                   // RESET STRING RECEIVED
}
