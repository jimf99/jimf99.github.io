    
              
              //    ARDUINO TO 433 TX
              //    This is part of an Instructable: http://www.instructables.com/member/InevitableCraftsLab/
              //    
              //    InevitableCraftsLab 2016
              //    
              //    Use it to send four analog values over 433MHz Tx/Rx



#include <VirtualWire.h>                                                                        //  INCLUDE THE VIRTUALWIRE LIBRARY
 
int Sensor1Pin = A1;                                                                            //  POT 1 ANALOG INPUT
int Sensor2Pin = A2;                                                                            //  POT 2 ANALOG INPUT
int Sensor3Pin = A3;                                                                            //  POT 3 ANALOG INPUT
int Sensor4Pin = A4;                                                                            //  POT 4 ANALOG INPUT


int Sensor1Data;                                                                                //  VARIABLE TO STORE THE ANALOG VALUE OF POT 1
int Sensor2Data;                                                                                //  VARIABLE TO STORE THE ANALOG VALUE OF POT 2
int Sensor3Data;                                                                                //  VARIABLE TO STORE THE ANALOG VALUE OF POT 3
int Sensor4Data;                                                                                //  VARIABLE TO STORE THE ANALOG VALUE OF POT 4

char Sensor1CharMsg[21];                                                                        //  ARRAY TO SAVE ALL FOUR ANALOG VARIABLES
 
void setup() {
 

 pinMode(Sensor1Pin,INPUT);                                                                     //  SET THE PIN OF POT 1 TO INPUT
 pinMode(Sensor2Pin,INPUT);                                                                     //  SET THE PIN OF POT 2 TO INPUT
 pinMode(Sensor3Pin,INPUT);                                                                     //  SET THE PIN OF POT 3 TO INPUT
 pinMode(Sensor4Pin,INPUT);                                                                     //  SET THE PIN OF POT 4 TO INPUT
  

 vw_setup(6000);                                                                                //  BAUDERATE FOR VIRTUAL WIRE CONNECTION
                                                                                                //  2000 was to jittery
                                                                                                //  9600 didnt work at all
                                                                                                //  dont forget to set the according baude rate on the receiver
 
 vw_set_tx_pin(12);                                                                                   //  PIN TO TX MODULE
 
}
 
void loop() {
   
  Sensor1Data = analogRead(Sensor1Pin);                                                         //  GET SENSORDATA 1 AND STORE IT 
  Sensor2Data = analogRead(Sensor2Pin);                                                         //  GET SENSORDATA 2 AND STORE IT 
  Sensor3Data = analogRead(Sensor3Pin);                                                         //  GET SENSORDATA 3 AND STORE IT 
  Sensor4Data = analogRead(Sensor4Pin);                                                         //  GET SENSORDATA 4 AND STORE IT 
   
  sprintf(Sensor1CharMsg, "%d,%d,%d,%d,", Sensor1Data, Sensor2Data, Sensor3Data, Sensor4Data);  //  STORE THE SENSORDATA IN AN ARRAY
  
 vw_send((uint8_t *)Sensor1CharMsg, strlen(Sensor1CharMsg));                                    //  MAKE A STRING OF THE ARRAY AND SEND IT 
 vw_wait_tx();                                                                                  //  WAIT TILL TRANSMISSION IS FINISHED

}
