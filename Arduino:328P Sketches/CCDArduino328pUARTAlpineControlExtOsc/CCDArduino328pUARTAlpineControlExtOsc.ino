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
  Left Middle - PRESET/ALBUM UP (actually goes down in the A-Z list.)


*/

//--------------------------Pin Assignment and Binary Coding for Alpine headunit-----------------------//
#define alpPin 4
#define idlePin 2
#define controlPin 3
#define ledPin 13

boolean volUp[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
boolean volDn[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
//boolean mute[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
boolean pstUp[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1};
boolean trkUp[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
boolean trkDn[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
//boolean entPlay[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};


/*****************************************************************************************************/
// Not in use / This function is performed by hardware Oscillator.
// 1MHz generator for CDP68HC68S1 IC
/*****************************************************************************************************/
/*
  #include <TimerOne.h> //https://pjrc.com/teensy/td_libs_TimerOne.html
  const int PWMPin = 9; //PWM~ Pin#
  const int Period = 1; // Period 1 us = 1Mhz, 2 us = 500 kHz, 3 us = 333 kHz, 40 us = 25 kHz
  const int dutyCycle = 512; // 0 - 1023   512 = dutyCycle 50
*/
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



const int numOfBytes = 8; // receive buffer size
/*  //left over from testing.
  int timeout = 4; // Serial1.setTimeout(timeout);
  volatile int BCounter = 0;// Software UART byte count
*/


#include <AltSoftSerial.h>
#include <avr/wdt.h>


//byte ISR_ccd_buff[numOfBytes]; /* CCD receive buffer inside ISR / MAX Bytes are in numOfBytes */
byte ccd_buff[numOfBytes]; /* CCD receive buffer / MAX Bytes are in numOfBytes */
int ccd_buff_ptr; /* CCD receive buffer pointer */
volatile byte IdleOnOffFlag = 0; //variable for the idle pin. Must be volatile due to being part of interrupt
//boolean DataComplete = false; //variable for declaring end of message string. Can be True or False.


AltSoftSerial mySerial; // RX, TX

void watchdogSetup() {
  cli(); // disable all interrupts
  wdt_reset(); // reset the WDT timer
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = (0 << WDIE) | (1 << WDE) | (0 << WDP3) | (1 << WDP2) | (1 << WDP1) | (0 << WDP0); //1000ms WDT timer
  sei();
}

void setup() {
  watchdogSetup();


  pinMode(idlePin, INPUT);            //set idle pin for input
  //pinMode(controlPin, INPUT);         //set control pin for input
  pinMode(ledPin, OUTPUT);            //prep builtin led pin
  pinMode(alpPin, OUTPUT);                 //Alpine control pin


  digitalWrite(ledPin, LOW);          //Set LED to low once. May be unnecessary but doesn't hurt anything.


  /*****************************************************************************************************/
  // Not in use / This function is performed by hardware Oscillator.
  // 1MHz generator for CDP68HC68S1 IC
  /*****************************************************************************************************/
  /*
    pinMode(PWMPin, OUTPUT);                 //prepare pin 9 for clock output
    // setup 1MHz generator for CDP68HC68S1 IC
    Timer1.initialize(Period);
    Timer1.pwm(PWMPin, dutyCycle); ////PWM~ Pin# 10
  */


  mySerial.begin(7812.5); //for serial IC
  //Serial.begin(57600);


  attachInterrupt(digitalPinToInterrupt(idlePin), endofstring, CHANGE);
  // high = BUSY , low = IDLE
}


void endofstring() {
  IdleOnOffFlag = digitalRead(idlePin);
  //IdleOnOffFlag = 1; //Sets flag high when idle pin goes high

}


/* ###################### (CRC) function #######################*/
uint8_t CyclicRedundancyCheck() {
  
  //Serial.println("CRC Function");
  // Cyclic Redundancy Check (CRC)
  // The checksum byte is calulated by summing all the ID and Data Bytes.
  // 256 is then subtracted from that sum.
  // The subtraction will end once the checksum falls within 0-255 decim


  if (ccd_buff_ptr >= 1) { //do not subtract if 0 ( BUG FIX )
    ccd_buff_ptr = ccd_buff_ptr - 1; //  subtract 1 from byte count [array indices usually start at 0]
  }
  uint16_t _CRC = 0; //was uint8_t _CRC = 0; bug fix
  for (uint16_t CRCptr = 0; CRCptr < ccd_buff_ptr; CRCptr ++) { // uint16_t 0 to 65,535 ~ int16_t -32,768 to 32,767
    _CRC = _CRC + ccd_buff[CRCptr];
  }
  while (_CRC > 255) {
    _CRC = _CRC - 256;
  }
  uint8_t x = _CRC;
  return x;
}


void loop() {
  // put your main code here, to run repeatedly:
  wdt_reset();

  //IdleOnOffFlag = digitalRead(idlePin);


  while (mySerial.available()) {
    ccd_buff[ccd_buff_ptr] = mySerial.read();
    //ccd_buff[ccd_buff_ptr] = data;    // read & store character
    ccd_buff_ptr++;                   // increment the pointer to the next byte
  }


  if (IdleOnOffFlag == 0) { // check the CDP68HC68S1 IDLE pin interrupt flag, change from Low to High.


    // Serial-x.readBytes(buffer, length)
    // Serial-x.readBytes() reads Bytes from the serial port into a buffer.
    // The function terminates if the determined length has been read, or it times out (see Serial.setTimeout()).
    // Serial.readBytes() returns the number of characters placed in the buffer. A 0 means no valid data was found.

    uint8_t CRC = 0;
    CRC = CyclicRedundancyCheck(); // Go get CRC.

    if (CRC != 0) {
      //Serial.print("CRC = #"); //Debug Serial prints
      //Serial.println(CRC);
      //Serial.print("CCD Pointer=");
      //Serial.println(ccd_buff_ptr);
      if (ccd_buff[ccd_buff_ptr] == CRC && ccd_buff_ptr != 0) {
        //Serial.println("Process");
        noInterrupts(); // Disables interrupts
        process_data(); // GOTO process_data loop
        ccd_buff_ptr = 0; // RESET buffer pointer to byte 0 for data to be overwritten
        interrupts();
      }
    }

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
  /*ccd_buff[0] = 0;
    ccd_buff[1] = 0;
    ccd_buff[2] = 0;
    ccd_buff[3] = 0;
    ccd_buff[4] = 0;
    ccd_buff[5] = 0;*/

}


//---------VOL UP-----------------------------------------------
void volUpSend() {
  digitalWrite(ledPin, HIGH);
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
  for (unsigned int i = 0; i <= 40; i++) { //New 41ms Delay
    delayMicroseconds(1000);
  }
  digitalWrite(ledPin, LOW);
}


//---------VOL DOWN-----------------------------------------------
void volDnSend() {
  digitalWrite(ledPin, HIGH);
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
  for (unsigned int i = 0; i <= 40; i++) { //New 41ms Delay
    delayMicroseconds(1000);
  }
  digitalWrite(ledPin, LOW);
}


//---------Next Track-----------------------------------------------
void trkUpSend() {
  digitalWrite(ledPin, HIGH);
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
  for (unsigned int i = 0; i <= 40; i++) { //New 41ms Delay
    delayMicroseconds(1000);
  }
  digitalWrite(ledPin, LOW);
}


//---------Previous Track----------------------------------------------
void trkDnSend() {
  digitalWrite(ledPin, HIGH);
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
  for (unsigned int i = 0; i <= 40; i++) { //New 41ms Delay
    delayMicroseconds(1000);
  }
  digitalWrite(ledPin, LOW);
}


//---------NEXT DOWN----------------------------------------------
void pstUpSend() {
  digitalWrite(ledPin, HIGH);
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
  for (unsigned int i = 0; i <= 40; i++) { //New 41ms Delay
    delayMicroseconds(1000);
  }
  digitalWrite(ledPin, LOW);
}
