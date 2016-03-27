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
//boolean pstDn[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1};
//boolean source[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1};
boolean trkUp[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
boolean trkDn[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
//boolean power[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1};
//boolean entPlay[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
//boolean bandProg[48] = {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1};

//#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
#include <avr/wdt.h>

byte ccd_buff[8]; /* CCD receive buffer / MAX 8 Bytes */
int ccd_buff_ptr; /* CCD receive buffer pointer */
volatile byte IdleOnOffFlag = 0; //variable for the idle pin. Must be volatile due to being part of interrupt
boolean DataComplete = false; //variable for declaring end of message string. Can be True or False.

//SoftwareSerial mySerial(6, 7); // RX, TX
AltSoftSerial mySerial; // RX, TX


void setup() {
 

  pinMode(idlePin, INPUT);            //set idle pin for input
  //pinMode(controlPin, INPUT);         //set control pin for input
  pinMode(ledPin, OUTPUT);            //prep builtin led pin
  digitalWrite(ledPin, LOW);          //Turn D13 LED Off
  pinMode(alpPin, OUTPUT);                 //Alpine control pin

  watchdogSetup();
 
  mySerial.begin(7812.5); //for serial IC

  attachInterrupt(digitalPinToInterrupt(idlePin), endofstring, RISING);
}
void watchdogSetup() {
  cli(); // disable all interrupts 
  wdt_reset(); // reset the WDT timer 
  WDTCSR |= (1<<WDCE) | (1<<WDE); 
  WDTCSR = (0<<WDIE) | (1<<WDE) | (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (0<<WDP0); //1000ms WDT timer
  sei();
}

void endofstring() {
  IdleOnOffFlag = 1; //Sets flag high when idle pin goes high
}

void loop() {
  wdt_reset();

  while (mySerial.available()) {
    byte data = mySerial.read();
    ccd_buff[ccd_buff_ptr] = data;    // read & store character
    ccd_buff_ptr++;                   // increment the pointer to the next byte
  }
  if (IdleOnOffFlag == 1) { // check the CDP68HC68S1 IDLE pin interrupt flag, change from Low to High.
    noInterrupts();
    digitalWrite(ledPin, HIGH); //heartbeat check
    process_data(); // GOTO process_data loop
    digitalWrite(ledPin, LOW); //heartbeat check
    IdleOnOffFlag = 0; //RESET IDLE PIN FLAG
    ccd_buff_ptr = 0; // RESET buffer pointer to byte 0 for data to be overwritten
    interrupts();
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
  for (unsigned int i = 0; i <= 41; i++) { //New 41ms Delay
    delayMicroseconds(1000);
  }
}

//---------VOL DOWN-----------------------------------------------
void volDnSend() {
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
  for (unsigned int i = 0; i <= 41; i++) { //New 41ms Delay
    delayMicroseconds(1000);
  }
}

//---------Next Track-----------------------------------------------
void trkUpSend() {
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
  for (unsigned int i = 0; i <= 41; i++) { //New 41ms Delay
    delayMicroseconds(1000);
  }
}

//---------Previous Track----------------------------------------------
void trkDnSend() {
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
  for (unsigned int i = 0; i <= 41; i++) { //New 41ms Delay
    delayMicroseconds(1000);
  }
}

//---------NEXT DOWN----------------------------------------------
void pstUpSend() {
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
  for (unsigned int i = 0; i <= 41; i++) { //New 41ms Delay
    delayMicroseconds(1000);
  }
}

