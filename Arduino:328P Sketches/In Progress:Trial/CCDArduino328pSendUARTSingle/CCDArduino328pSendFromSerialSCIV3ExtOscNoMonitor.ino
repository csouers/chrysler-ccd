/*
  SCI Code - Send Command

  By Chris Souers

  Sends commands
*/


volatile int readytosend = 0; //set to 1 to send data
volatile byte send_buff[5]; /* CCD send buffer / MAX 4-5 Bytes */
volatile int  send_buff_ptr; /* CCD send buffer pointer */

#define idlePin 2 //INT0 on 328P
#define controlPin 3 //INT1 on 328P
#define ledPin 13

#include <AltSoftSerial.h>

volatile byte ccd_buff[5]; /* CCD receive buffer / MAX 8 Bytes */
volatile int  ccd_buff_ptr; /* CCD receive buffer pointer */
volatile int IdleOnOffFlag = 1; //variable for the idle pin. Must be volatile due to being part of interrupt

AltSoftSerial mySerial; // RX, TX //SoftwareSerial may work better for SBIC - pins (6,7);


void setup() {

  pinMode(idlePin, INPUT);            //set idle pin for input
  pinMode(ledPin, OUTPUT);            //prep builtin led pin
  digitalWrite(ledPin, LOW);          //Turn D13 LED Off

  mySerial.begin(7812.5); //for serial IC
  Serial.begin(57600); //for serial monitor
  Serial.println("Serial Prompt Ready - GO");
  delay(1000);

  //attachInterrupt(digitalPinToInterrupt(idlePin), endofstring, CHANGE);
}


void endofstring() {

  IdleOnOffFlag = digitalRead(idlePin);
  //digitalWrite(ledPin, IdleOnOffFlag);

}


void loop() {

  if (Serial.available() > 0) { //reads 4 data bytes in, plus 1 for the ready switches
    for (send_buff_ptr = 0; send_buff_ptr <= 2; send_buff_ptr++) {
      send_buff[send_buff_ptr] = Serial.parseInt();
    }
    readytosend = Serial.parseInt();
    ccd_buff_ptr = 0;
    send_buff_ptr = 0;
  }
  endofstring(); //continually checks for idle flag state

  if (IdleOnOffFlag == 0 && readytosend == 1) {

    send_data();

  }

}

void send_data() {

  while (mySerial.available()) {
    if (send_buff_ptr == 0) {
    delayMicroseconds(256); //uart loading delay maybe not necessary. Can be checked with scope.
    }

    mySerial.write(send_buff[send_buff_ptr]);

    ccd_buff[ccd_buff_ptr] = mySerial.read();

    if (send_buff_ptr == 0 && send_buff[send_buff_ptr] != ccd_buff[ccd_buff_ptr]) {
      data_reset();
      break;
    }

    if (send_buff_ptr >= 1 && send_buff[send_buff_ptr] != ccd_buff[ccd_buff_ptr]) {
      delayMicroseconds(1280); //EOM software delay
      data_reset();
      break;
    }

    if (send_buff[send_buff_ptr] == ccd_buff[ccd_buff_ptr]) {
      ccd_buff_ptr++;
      send_buff_ptr++;
    }

    if (send_buff_ptr >= 4) {
      readytosend = 0;
      delayMicroseconds(1280); //EOM software delay
      data_reset();
      break;
    }

  }
}

void data_reset() {
  Serial.println("Reset");

  /*
    if (ccd_buff_ptr >= 1) {
      readytosend = 0;
      for (ccd_buff_ptr = 0; ccd_buff_ptr <= 4; ccd_buff_ptr++) {
        ccd_buff[ccd_buff_ptr] = 0;
      }

      for (send_buff_ptr = 0; send_buff_ptr <= 4; send_buff_ptr++) {
        send_buff[send_buff_ptr] = 0;
      }
    }
  */
  //delay(25);
  ccd_buff_ptr = 0;
  send_buff_ptr = 0;


}





