/*
  Code adapted from Chris O. and Juha Niinikoski

  Prints ID Byte, sent via serial monitor from PC, to serial monitor.
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
#include <avr/wdt.h>

int idlePin = 2;      //Idle pin /INT0 on Atmega328P
int controlPin = 3;   //Control pin /INT1 on Atmega328P. Not being used as an interrupt right now
int ledPin = 13;      //Onboard LED pin incase we need it

byte ccd_buff[8]; /* CCD receive buffer / MAX 8 Bytes */
int ccd_buff_ptr; /* CCD receive buffer pointer */
volatile byte IdleOnOffFlag = 0; //variable for the idle pin. Must be volatile due to being part of interrupt
boolean DataComplete = false; //variable for declaring end of message string. Can be True or False.

SoftwareSerial mySerial(6, 7); // RX, TX

volatile byte selectbyte = 68; //ID byte to print to the serial monitor

void setup() {


  pinMode(PWMPin, OUTPUT);                 //prepare pin 9 for clock output
  pinMode(idlePin, INPUT);            //set idle pin for input
  //pinMode(controlPin, INPUT);         //set control pin for input
  pinMode(ledPin, OUTPUT);            //prep builtin led pin
  pinMode(4, OUTPUT);                 //Alpine control pin

  digitalWrite(ledPin, LOW);          //Set LED to low once. May be unnecessary but doesn't hurt anything.
  //watchdogSetup();
  // setup 1MHz generator for CDP68HC68S1 IC
  Timer1.initialize(Period);
  Timer1.pwm(PWMPin, dutyCycle); ////PWM~ Pin# 10
  Serial.begin(57600);
  mySerial.begin(7812.5); //for serial IC

  attachInterrupt(digitalPinToInterrupt(idlePin), endofstring, RISING);
}
void watchdogSetup() {
  cli(); // disable all interrupts
  wdt_reset(); // reset the WDT timer
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = (0 << WDIE) | (1 << WDE) | (0 << WDP3) | (1 << WDP2) | (1 << WDP1) | (0 << WDP0); //1000ms WDT timer
  sei();
}

void endofstring() {
  IdleOnOffFlag = 1; //Sets flag high when idle pin goes high
}

void loop() {
  wdt_reset();
  if (Serial.available() > 0) {
    selectbyte = Serial.parseInt();
  }

  while (mySerial.available()) {

    byte data = mySerial.read();
    ccd_buff[ccd_buff_ptr] = data;    // read & store character
    ccd_buff_ptr++;                   // increment the pointer to the next byte
  }
  if (IdleOnOffFlag == 1) { // check the CDP68HC68S1 IDLE pin interrupt flag, change from Low to High.
    process_data(); // GOTO process_data loop
    IdleOnOffFlag = 0; //RESET IDLE PIN FLAG
    ccd_buff_ptr = 0; // RESET buffer pointer to byte 0 for data to be overwritten
  }
}

void process_data() {
    if (ccd_buff[0] == selectbyte) {

    Serial.print((int)ccd_buff[0]);
    Serial.print(" ");
    Serial.print((int)ccd_buff[1]);
    Serial.print(" ");
    Serial.print((int)ccd_buff[2]);
    Serial.print(" ");
    Serial.print((int)ccd_buff[3]);
    Serial.print(" ");
    Serial.print((int)ccd_buff[4]);
    Serial.print(" ");
    Serial.println((int)ccd_buff[5]);

  ccd_buff[0] = 0;
  ccd_buff[1] = 0;
  ccd_buff[2] = 0;
  ccd_buff[3] = 0;
  ccd_buff[4] = 0;
  ccd_buff[5] = 0;
    }
    
  else{
  ccd_buff[0] = 0;
  ccd_buff[1] = 0;
  ccd_buff[2] = 0;
  ccd_buff[3] = 0;
  ccd_buff[4] = 0;
  ccd_buff[5] = 0;
}
}


