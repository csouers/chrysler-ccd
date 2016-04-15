/*
  SCI Code - Send Command

  By Chris Souers

  Sends commands
*/


volatile int readytosend = 0; //set to 1 to send data
volatile byte send_buff[4]; /* CCD send buffer / MAX 4-5 Bytes */
volatile int  send_buff_ptr; /* CCD send buffer pointer */

#define idlePin 2 //INT0 on 328P
#define controlPin 3 //INT1 on 328P
#define ledPin 13

#include <AltSoftSerial.h>
//#include <SoftwareSerial.h>

volatile byte ccd_buff[4]; /* CCD receive buffer / MAX 8 Bytes */
volatile int  ccd_buff_ptr; /* CCD receive buffer pointer */
volatile int IdleOnOffFlag = 1; //variable for the idle pin. Must be volatile due to being part of interrupt

AltSoftSerial mySerial; // RX, TX //SoftwareSerial may work better for SBIC - pins (6,7);
//SoftwareSerial mySerial (8, 9); // RX, TX //SoftwareSerial may work better for SBIC - pins (6,7);


void setup() {

  pinMode(idlePin, INPUT);            //set idle pin for input
  pinMode(ledPin, OUTPUT);            //prep builtin led pin
  digitalWrite(ledPin, LOW);          //Turn D13 LED Off

  mySerial.begin(7812.5); //for serial IC
  Serial.begin(115200); //for serial monitor
  Serial.println("Serial Prompt Ready - GO");

  //attachInterrupt(digitalPinToInterrupt(idlePin), endofstring, CHANGE);
}


void endofstring() {

  IdleOnOffFlag = digitalRead(idlePin);
  //digitalWrite(ledPin, IdleOnOffFlag);

}


void loop() {

  if (Serial.available() > 0) { //reads 4 data bytes in, plus 1 for the ready switches
    for (send_buff_ptr = 0; send_buff_ptr <= 3; send_buff_ptr++) {
      send_buff[send_buff_ptr] = Serial.parseInt();
    }
    readytosend = Serial.parseInt();
    ccd_buff_ptr = 0;
    send_buff_ptr = 0;
  }

  endofstring(); //continually checks for idle flag state

  if (IdleOnOffFlag == 0 && readytosend == 1) {
    Serial.println();
    Serial.println("--Sending Data--");
    //send_data();
    mySerial.write(send_buff[0]);
      mySerial.write(send_buff[1]);


      mySerial.write(send_buff[2]);


      mySerial.write(send_buff[3]);

      delay(1000);
      //readytosend = 0;

  }

}

void send_data() {

  if (send_buff_ptr == 0) {
    delayMicroseconds(256);
    Serial.println("Arbitration Delay 256us");
  }

  Serial.print("Sending Byte #");
  Serial.println(send_buff[send_buff_ptr]);
  mySerial.write(send_buff[send_buff_ptr]);

  //delayMicroseconds(2000); //delay between send and receive

  ccd_buff[ccd_buff_ptr] = mySerial.read();
  Serial.print("Got CCD Byte #");
  Serial.println(ccd_buff[ccd_buff_ptr]);


  if (send_buff_ptr >= 0 && send_buff[send_buff_ptr] != ccd_buff[ccd_buff_ptr]) {
    Serial.println("Buffer Mismatch - Aborting");
    Serial.println("EOM Delay");
    delayMicroseconds(1280); //EOM software delay
    data_reset();
    return;
  }



  if (send_buff[send_buff_ptr] == ccd_buff[ccd_buff_ptr] && send_buff_ptr <= 3) {
    Serial.println("Successful. Attempting next byte.");
    ccd_buff_ptr++;
    send_buff_ptr++;
    return;
  }

  if (send_buff_ptr >= 4) {
    data_reset();
  }
}



void data_reset() {
  Serial.println("Resetting system");

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
  ccd_buff_ptr = 0;
  send_buff_ptr = 0;
  delay(100);


}





