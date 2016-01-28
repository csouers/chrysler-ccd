/*
SCI Code - Send Command

By Chris Souers

Sends commands without regard to if the data was seen on the bus or not. Does not check for reflected bytes as it should. Needs improved upon, but works okay for now. Should not be a problem when bus is idle/asleep.

*/

// 1MHz generator for CDP68HC68S1 IC
#include <TimerOne.h> //https://pjrc.com/teensy/td_libs_TimerOne.html
const int PWMPin = 9; //PWM~ Pin#
const int Period = 1; // Period 1 us = 1Mhz, 2 us = 500 kHz, 3 us = 333 kHz, 40 us = 25 kHz
const int dutyCycle = 512; // 0 - 1023   512 = dutyCycle 50
/* Download:  Included with the Teensyduino Installer
Latest TimerOne on Github https://github.com/PaulStoffregen/TimerOne
Latest TimerThree on Github https://github.com/PaulStoffregen/TimerThree
Hardware Requirements
These libraries use Timer1 and Timer3.
Each timer controls PWM pins. While uses these libraries, analogWrite() to those pins will not work normally,
but you can use the library pwm() function.


Board ------ TimerOne PWM Pins  
Teensy 3.1 ------- 3, 4 
Teensy 3.0 ------- 3, 4  
Teensy 2.0 ------- 4, 14, 15
Teensy++ 2.0 ----- 25, 26, 27
Arduino Uno ------ 9, 10
Arduino Leonardo - 9, 10, 11
Arduino Mega ----- 11, 12, 13
Wiring-S --------- 4, 5  
Sanguino --------- 12, 13


Board ------ TimerThree PWM Pins
Teensy 3.1 ------- 25, 32
Teensy 2.0 ------- 9
Teensy++ 2.0 ----- 14, 15, 16
Arduino Leonardo - 5
Arduino Mega ----- 2, 3, 5
/ 1MHz generator */

#include <SoftwareSerial.h>


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

int inbyte = 0; //Data coming into bus

SoftwareSerial mySerial(6, 7); // RX, TX

void setup() {

  pinMode(PWMPin, OUTPUT);                //prepare pin 9 for clock output
  pinMode(idlePin, INPUT);            //set idle pin for input
  pinMode(controlPin, INPUT);         //set control pin for input
  pinMode(ledPin, OUTPUT);                //prep builtin led pin


  Serial.begin(57600); //have had issues at 115200, so 57600 seems to work fine.
  mySerial.begin(7812.5); //for serial IC

  // setup 1MHz generator for CDP68HC68S1 IC
  Timer1.initialize(Period);
  Timer1.pwm(PWMPin, dutyCycle); ////PWM~ Pin# 9


  //attachInterrupt(digitalPinToInterrupt(triggerPin), send, CHANGE);


}

void loop() {
  if (Serial.available() > 0) {
    byte1 = Serial.parseInt();
    byte2 = Serial.parseInt();
    byte3 = Serial.parseInt();
    byte4 = Serial.parseInt();
    byte5 = Serial.parseInt();
    byte6 = Serial.parseInt();
    readytosend = Serial.parseInt();
    
    //To send data, open serial monitor. Input data string, in decimal, with spaces in between each followed by 1.
    //Example: "206 116 151 238 190 133 1"
  }

  delay(100);
  idleStatus = digitalRead(idlePin);
  if (idleStatus == 0 && readytosend == 1) {
    mySerial.write(byte1);        //sends the first byte
    mySerial.write(byte2);
    mySerial.write(byte3);
    mySerial.write(byte4);
    mySerial.write(byte5);
    mySerial.write(byte6);
    readytosend = 1; //1 will continually send data. Change to 0 for sending only once per serial input

  }
}

/*void send() {
  idleStatus = digitalRead(idlePin);
  if (idleStatus == 0) {
    mySerial.write(byte1);        //sends the first byte
    mySerial.write(byte2);
    mySerial.write(byte3);
    mySerial.write(byte4);
    mySerial.write(byte5);
    mySerial.write(byte6);
    delay(2);


  }

}

*/
