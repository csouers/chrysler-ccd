/*
SCI Code - Send Command

By Chris Souers

Sends commands without regard to if the data was seen on the bus or not. Does not check for reflected bytes as it should. Needs improved upon, but works okay for now. Should not be a problem when bus is idle/asleep.

*/


#include <AltSoftSerial.h>


int idlePin = 2;      //Idle pin /INT0
int controlPin = 3;
int ledPin = 13;

int idleStatus = 0;
int ledStatus = 0;
int readytosend = 0;

byte byte1 = 0; //Data to send
byte byte2 = 0;
byte byte3 = 0;
byte byte4 = 0;
byte byte5 = 0;
byte byte6 = 0;


AltSoftSerial mySerial; // RX, TX

void setup() {

  pinMode(idlePin, INPUT);            //set idle pin for input
  pinMode(controlPin, INPUT);         //set control pin for input
  pinMode(ledPin, OUTPUT);                //prep builtin led pin


  Serial.begin(57600); //have had issues at 115200, so 57600 seems to work fine.
  mySerial.begin(7812.5); //for serial IC


  //attachInterrupt(digitalPinToInterrupt(triggerPin), send, CHANGE);


}

void loop() {
  if (Serial.available() > 0) {
    byte1 = Serial.parseInt();
    byte2 = Serial.parseInt();
    byte3 = Serial.parseInt();
    byte4 = Serial.parseInt();
    //byte5 = Serial.parseInt();
    //byte6 = Serial.parseInt();
    readytosend = Serial.parseInt();
    
    //To send data, open serial monitor. Input data string, in decimal, with spaces in between each followed by 1.
    //Example: "206 116 151 238 190 133 1"
  }

  //delay(100);
  idleStatus = digitalRead(idlePin);
  if (idleStatus == 0 && readytosend == 1) {
    mySerial.write(byte1);        //sends the first byte
    mySerial.write(byte2);
    mySerial.write(byte3);
    mySerial.write(byte4);
    //mySerial.write(byte5);
   // mySerial.write(byte6);
    readytosend = 0; //1 will continually send data. Change to 0 for sending only once per serial input

  }
}
