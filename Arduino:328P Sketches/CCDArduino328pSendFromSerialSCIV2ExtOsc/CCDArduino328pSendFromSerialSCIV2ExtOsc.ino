/*
  SCI Code - Send Command

  By Chris Souers

  Sends commands
*/


byte readytosend = 0; //set to 1 to send data
byte send_buff[4]; /* CCD receive buffer / MAX 8 Bytes */
int send_buff_ptr; /* CCD receive buffer pointer */

#define idlePin 2 //INT0 on 328P
#define controlPin 3 //INT1 on 328P
#define ledPin 13

#include <AltSoftSerial.h>
#include <avr/wdt.h>

byte ccd_buff[8]; /* CCD receive buffer / MAX 8 Bytes */
int ccd_buff_ptr; /* CCD receive buffer pointer */
volatile byte IdleOnOffFlag = 0; //variable for the idle pin. Must be volatile due to being part of interrupt
boolean DataComplete = false; //variable for declaring end of message string. Can be True or False.

AltSoftSerial mySerial; // RX, TX //SoftwareSerial may work better for SBIC - pins (6,7);


void setup() {


  pinMode(idlePin, INPUT);            //set idle pin for input
  pinMode(ledPin, OUTPUT);            //prep builtin led pin
  digitalWrite(ledPin, LOW);          //Turn D13 LED Off

  watchdogSetup();

  mySerial.begin(7812.5); //for serial IC
  Serial.begin(57600); //for serial monitor

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
  if (Serial.available() > 0) { //reads 4 bytes in, plus 1 byte for the ready switch
    for (send_buff_ptr = 0; send_buff_ptr <= 3; send_buff_ptr++) {
      send_buff[send_buff_ptr] = Serial.parseInt();

    }
    readytosend = Serial.parseInt();
  }
  send_buff_ptr = 0;

  noInterrupts();

  if (IdleOnOffFlag == 1 && readytosend == 1) {
    ccd_buff_ptr = 0;
    send_buff_ptr = 0;
    Serial.println("Sending Message");
    send_command(); 
  }
  interrupts();
}
void send_command(){
sending:

  Serial.print("Sending byte #");
  Serial.println(send_buff[send_buff_ptr]);
  mySerial.write(send_buff[send_buff_ptr]);


  if (mySerial.available()) {
    byte data = mySerial.read();
    ccd_buff[ccd_buff_ptr] = data;    // read & store character
    Serial.print("Got byte #");
    Serial.println(ccd_buff[ccd_buff_ptr]);
  }
  if (send_buff[send_buff_ptr] = ccd_buff[ccd_buff_ptr]) {
    ccd_buff_ptr++;                   // increment the pointers to the next byte
    send_buff_ptr++;
    goto sending;

  }

  if (send_buff[send_buff_ptr] != ccd_buff[ccd_buff_ptr]) {
    Serial.println("Message Sequence Aborted - Back to loop");
    //loop();
    return;
  }


  if (send_buff_ptr = 4) {
    readytosend = 0; //set flag to 0, sending command string one time
    ccd_buff_ptr = 0;
    send_buff_ptr = 0;
    Serial.println("Message Sequence Completed");
    IdleOnOffFlag = 0; //reset idle flag

    //loop();
    return;

  }
  else {

    send_command();
  }

}




