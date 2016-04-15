/*
  SCI Code - Read to Serial
*/

#include <AltSoftSerial.h>


int idlePin = 2;      //Idle pin /INT0
int controlPin = 3;
int ledPin = 13;


AltSoftSerial mySerial; // RX, TX

void setup() {

  pinMode(idlePin, INPUT);            //set idle pin for input
  pinMode(controlPin, INPUT);         //set control pin for input
  pinMode(ledPin, OUTPUT);                //prep builtin led pin


  Serial.begin(57600); //have had issues at 115200, so 57600 seems to work fine.
  mySerial.begin(7812.5); //for serial IC


  attachInterrupt(digitalPinToInterrupt(idlePin), endofstring, CHANGE);

}


void loop() {
  if (mySerial.available()) { //Waits for available data

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


void endofstring() {
  Serial.print("\n");
} //When IDLE changes, print a new line. Two new lines per message string. One high to low, another low to high. Have had issues with only going to low and strings running together. Likely a timing/speed issue.




