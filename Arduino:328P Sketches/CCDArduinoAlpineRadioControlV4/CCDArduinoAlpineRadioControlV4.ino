/*

  Alpine Signaling and Code from mp3car.com - http://www.mp3car.com/vbulletin/input-devices/131147-need-alpine-wired-remote-control-pinout-2.html#post1341875

  Utilizes standard 3 wire 3.5mm connection for the Head Unit. Uses tip and base conductors. Use 220ohm resistor on signal connection to prevent too much current draw at Arduino Pin.
  CCD communcation via CDP68HC68S1

  Code adapted from Chris O. and Juha Niinikoski

  -ZJ Steering Control Layout-

  Right Up - VOL UP
  Right Down - VOL DOWN
  Left Up - NEXT TRACK
  Left Down - PREVIOUS TRACK
  Left Middle - PRESET/ALBUM UP (or down? Will have to check functionality on car and make appropriate changes.)

  Update Jan 22 - This code works, but with interruptions from the IDLE pin.
  Thus, I'm assuming Alpine commands aren't received in their entirety and nothing happens.
  Less bus traffic helps, key in accessory for example.
  Code verfied with LED functionality. Works as expected
  May need to offload Alpine functions to separate microcontroller communication with the CCD Module over UART or I2c.


*/
//--------------------------Pin Assignment and Binary Coding for Alpine headunit-----------------------//
#define alpPin 4

boolean volUp[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
boolean volDn[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
//boolean mute[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
boolean pstUp[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1};
//boolean pstDn[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1};
//boolean source[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1};
boolean trkUp[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
boolean trkDn[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
//boolean power[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1};
//boolean entPlay[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
//boolean bandProg[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1};

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

int idlePin = 2;      //Idle pin /INT0 on Atmega328P
int controlPin = 3;   //Control pin /INT1 on Atmega328P. Not being used as an interrupt right now
int ledPin = 13;      //Onboard LED pin incase we need it

byte ccd_buff[8]; /* CCD receive buffer / MAX 8 Bytes */
int ccd_buff_ptr; /* CCD receive buffer pointer */
volatile byte IdleOnOffFlag = 0; //variable for the idle pin. Must be volatile due to being part of interrupt
boolean DataComplete = false; //variable for declaring end of message string. Can be True or False.

SoftwareSerial mySerial(6, 7); // RX, TX

void setup() {
 

  pinMode(PWMPin, OUTPUT);                 //prepare pin 9 for clock output
  pinMode(idlePin, INPUT);            //set idle pin for input
  //pinMode(controlPin, INPUT);         //set control pin for input
  pinMode(ledPin, OUTPUT);            //prep builtin led pin
  pinMode(4, OUTPUT);                 //Alpine control pin

  digitalWrite(ledPin, LOW);          //Set LED to low once. May be unnecessary but doesn't hurt anything.

   // setup 1MHz generator for CDP68HC68S1 IC
  Timer1.initialize(Period);
  Timer1.pwm(PWMPin, dutyCycle); ////PWM~ Pin# 10

  mySerial.begin(7812.5); //for serial IC

  attachInterrupt(digitalPinToInterrupt(idlePin), endofstring, RISING);
}

void endofstring() {
  IdleOnOffFlag = 1; //Sets flag high when idle pin goes high
}

void loop() {
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

  switch (ccd_buff[0])          // decide what to do from first byte / ID BYTE
  {

    case 0x82:        // Do Radio/VIC Time Commands = 130 DEC


      if ((ccd_buff[1] == 0x20) && (ccd_buff[2] == 0x20) && (ccd_buff[3] == 0x00) && (ccd_buff[4] == 0xC2)) { 
        //VOLUME UP
        volUpSend();
        break;
      }
      
      if ((ccd_buff[1] == 0x20) && (ccd_buff[2] == 0x40) && (ccd_buff[3] == 0x00) && (ccd_buff[4] == 0xE2)) { 
        //VOLUME DOWN
        volDnSend();
        break;
      }
      
      if ((ccd_buff[1] == 0x20) && (ccd_buff[2] == 0x04) && (ccd_buff[3] == 0x00) && (ccd_buff[4] == 0xA6)) { 
        //TRACK UP
        trkUpSend();
        break;
      }
      
      if ((ccd_buff[1] == 0x20) && (ccd_buff[2] == 0x08) && (ccd_buff[3] == 0x00) && (ccd_buff[4] == 0xAA)) { 
        //TRACK DOWN
        trkDnSend();
        break;
      }
      
      if ((ccd_buff[1] == 0x20) && (ccd_buff[2] == 0x00) && (ccd_buff[3] == 0x01) && (ccd_buff[4] == 0xA3)) { 
        //ALBUM UP
        pstUpSend();
        break;
      }
      
    default:
      //do nothing
      break;

  }
  ccd_buff[0] = 0;
  ccd_buff[1] = 0;
  ccd_buff[2] = 0;
  ccd_buff[3] = 0;
  ccd_buff[4] = 0;
  ccd_buff[5] = 0;
}

//---------VOL UP-----------------------------------------------
void volUpSend() {
  noInterrupts(); // Disables interrupts
  digitalWrite(ledPin,HIGH);
  //first send 8ms high
  digitalWrite(alpPin, HIGH);
  delayMicroseconds(8000); // New 8ms Delay
  // send 4.5ms low
  digitalWrite(alpPin, LOW);
  delayMicroseconds(4500);

  for (int i = 0; i <= 47; i++) {
    //send bit for 0.5ms
    if (volUp[i] == 1 ) {
      digitalWrite(alpPin, HIGH);
    } else {
      digitalWrite(alpPin, LOW);
    }
    delayMicroseconds(500);
    // wait 0.5ms
    digitalWrite(alpPin, LOW);
    delayMicroseconds(500);
  }
  // send 41ms low
  digitalWrite(alpPin, LOW);
  /*for (unsigned int i = 0; i <= 41; i++) { //New 41ms Delay
    delayMicroseconds(1000);
  }*/
  digitalWrite(ledPin,LOW);
  interrupts(); // Re-enables interrupts
}

//---------VOL DOWN-----------------------------------------------
void volDnSend() {
  noInterrupts(); // Disables interrupts
  digitalWrite(ledPin,HIGH);
  //first send 8ms high
  digitalWrite(alpPin, HIGH);
  delayMicroseconds(8000); // New 8ms Delay
  // send 4.5ms low
  digitalWrite(alpPin, LOW);
  delayMicroseconds(4500);

  for (int i = 0; i <= 47; i++) {
    //send bit for 0.5ms
    if (volDn[i] == 1 ) {
      digitalWrite(alpPin, HIGH);
    } else {
      digitalWrite(alpPin, LOW);
    }
    delayMicroseconds(500);
    // wait 0.5ms
    digitalWrite(alpPin, LOW);
    delayMicroseconds(500);
  }
  // send 41ms low
  digitalWrite(alpPin, LOW);
  /*for (unsigned int i = 0; i <= 41; i++) { //New 41ms Delay
    delayMicroseconds(1000);
  }*/
  digitalWrite(ledPin,LOW);
  interrupts(); // Re-enables interrupts
}

//---------Next Track-----------------------------------------------
void trkUpSend() {
  noInterrupts(); // Disables interrupts
  digitalWrite(ledPin,HIGH);
  //first send 8ms high
  digitalWrite(alpPin, HIGH);
  delayMicroseconds(8000); // New 8ms Delay
  // send 4.5ms low
  digitalWrite(alpPin, LOW);
  delayMicroseconds(4500);

  for (int i = 0; i <= 47; i++) {
    //send bit for 0.5ms
    if (trkUp[i] == 1 ) {
      digitalWrite(alpPin, HIGH);
    } else {
      digitalWrite(alpPin, LOW);
    }
    delayMicroseconds(500);
    // wait 0.5ms
    digitalWrite(alpPin, LOW);
    delayMicroseconds(500);
  }
  // send 41ms low
  digitalWrite(alpPin, LOW);
  /*for (unsigned int i = 0; i <= 41; i++) { //New 41ms Delay
    delayMicroseconds(1000);
  }*/
  digitalWrite(ledPin,LOW);
  interrupts(); // Re-enables interrupts
}

//---------Previous Track----------------------------------------------
void trkDnSend() {
  noInterrupts(); // Disables interrupts
  digitalWrite(ledPin,HIGH);
  //first send 8ms high
  digitalWrite(alpPin, HIGH);
  delayMicroseconds(8000); // New 8ms Delay
  // send 4.5ms low
  digitalWrite(alpPin, LOW);
  delayMicroseconds(4500);

  for (int i = 0; i <= 47; i++) {
    //send bit for 0.5ms
    if (trkDn[i] == 1 ) {
      digitalWrite(alpPin, HIGH);
    } else {
      digitalWrite(alpPin, LOW);
    }
    delayMicroseconds(500);
    // wait 0.5ms
    digitalWrite(alpPin, LOW);
    delayMicroseconds(500);
  }
  // send 41ms low
  digitalWrite(alpPin, LOW);
  /*for (unsigned int i = 0; i <= 41; i++) { //New 41ms Delay
    delayMicroseconds(1000);
  }*/
  digitalWrite(ledPin,LOW);
  interrupts(); // Re-enables interrupts
}

//---------NEXT DOWN----------------------------------------------
void pstUpSend() {
  noInterrupts(); // Disables interrupts
  digitalWrite(ledPin,HIGH);
  //first send 8ms high
  digitalWrite(alpPin, HIGH);
  delayMicroseconds(8000); // New 8ms Delay
  // send 4.5ms low
  digitalWrite(alpPin, LOW);
  delayMicroseconds(4500);

  for (int i = 0; i <= 47; i++) {
    //send bit for 0.5ms
    if (pstUp[i] == 1 ) {
      digitalWrite(alpPin, HIGH);
    } else {
      digitalWrite(alpPin, LOW);
    }
    delayMicroseconds(500);
    // wait 0.5ms
    digitalWrite(alpPin, LOW);
    delayMicroseconds(500);
  }
  // send 41ms low
  digitalWrite(alpPin, LOW);
  /*for (unsigned int i = 0; i <= 41; i++) { //New 41ms Delay
    delayMicroseconds(1000);
  }*/
  digitalWrite(ledPin,LOW);
  interrupts(); // Re-enables interrupts
}

