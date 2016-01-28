/*
SCI Code - Read

By Chris Souers

Reads data over UART. Sends it to USB via SoftwareSerial.

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

  attachInterrupt(digitalPinToInterrupt(idlePin), endofstring, CHANGE);
  
}


void loop() {
  digitalWrite(ledPin, LOW);
if (mySerial.available()){ //Waits for available data
  
Serial.print(mySerial.read()); //Prints the data as it comes in
Serial.print(" "); //Prints a space as a delimiter

  }
/*if (mySerial.read()) == "130,32,32,0,194,"{
  digitalWrite(ledPin, HIGH);

  else

  digitalWrite(ledPin, LOW);
}
*/
}


void endofstring(){ Serial.print("\n");
  digitalWrite(ledPin, HIGH);
} //When IDLE changes, print a new line. Two new lines per message string. One high to low, another low to high. Have had issues with only going to low and strings running together. Likely a timing/speed issue.




