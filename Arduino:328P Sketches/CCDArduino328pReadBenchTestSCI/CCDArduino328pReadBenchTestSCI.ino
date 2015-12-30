/*
Use Timer/Counter1 to generate a 1MHz square wave on Arduino pin 9.
J.Christensen 27Apr2012

SCI Code Chris Souers

Nov 13 2015 - Setup 1MHz square wave generation (untested) and Initial SCI/UART setup.
Nov 22 2015 - Added Code for Idle and Control Pin monitoring and Serial Printing
Nov 23 2015 - Initial practical testing. Assigned D9 as output to stop serial bus crashing.


*/

#include <SoftwareSerial.h>


int idlePin = 2;      //Idle pin /INT0
int controlPin = 3;
int ledPin = 13;

SoftwareSerial mySerial(6, 7); // RX, TX

void setup() {
 
  pinMode(9, OUTPUT);                //prepare pin 9 for clock output
  pinMode(idlePin, INPUT);            //set idle pin for input
  pinMode(controlPin, INPUT);         //set control pin for input
  pinMode(ledPin, OUTPUT);                //prep builtin led pin


  Serial.begin(57600);
  mySerial.begin(7812.5);
 
  DDRB = _BV(DDB1);                  //set OC1A/PB1 as output (Arduino pin D9, DIP pin 15)
  TCCR1A = _BV(COM1A0);              //toggle OC1A on compare match
  OCR1A = 7;                         //top value for counter
  TCCR1B = _BV(WGM12) | _BV(CS10);   //CTC mode, prescaler clock/1
  
  attachInterrupt(digitalPinToInterrupt(idlePin), endofstring, CHANGE);
  
}


void loop() {
if (mySerial.available()){
  
Serial.print(mySerial.read());
Serial.print(" ");

  }
/*if (mySerial.read()) == "130,32,32,0,194,"{
  digitalWrite(ledPin, HIGH);

  else

  digitalWrite(ledPin, LOW);
}
*/
}


void endofstring(){ Serial.print("\n"); }




