/*
  Code adapted from Chris O. and Juha Niinikoski

  Prints ID Byte, sent via serial monitor from PC, to serial monitor.
*/




#include <AltSoftSerial.h>
#include <avr/wdt.h>

int idlePin = 2;      //Idle pin /INT0 on Atmega328P
int controlPin = 3;   //Control pin /INT1 on Atmega328P. Not being used as an interrupt right now
int ledPin = 13;      //Onboard LED pin incase we need it

byte ccd_buff[8]; /* CCD receive buffer / MAX 8 Bytes */
int ccd_buff_ptr; /* CCD receive buffer pointer */
volatile byte IdleOnOffFlag = 0; //variable for the idle pin. Must be volatile due to being part of interrupt
boolean DataComplete = false; //variable for declaring end of message string. Can be True or False.

AltSoftSerial mySerial(6, 7); // RX, TX

volatile byte selectbyte = 134; //ID byte to print to the serial monitor

void setup() {


  pinMode(idlePin, INPUT);            //set idle pin for input
  //pinMode(controlPin, INPUT);         //set control pin for input
  pinMode(ledPin, OUTPUT);            //prep builtin led pin
  pinMode(4, OUTPUT);                 //Alpine control pin

  digitalWrite(ledPin, LOW);          //Set LED to low once. May be unnecessary but doesn't hurt anything.
  //watchdogSetup();
  // setup 1MHz generator for CDP68HC68S1 IC
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


