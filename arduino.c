/*
Team Safety Lab for PPE enforcement

*/

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

#define STATE_OPENED 0
#define STATE_CLOSED 1
#define STATE_OK 2


int state = STATE_OPENED;
int previous_state = -1;

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(13, OUTPUT);   
 
  Serial.begin(9600);	// Initialize serial communications with the PC
  SPI.begin();			// Init SPI bus
  mfrc522.PCD_Init();	// Init MFRC522 card
  Serial.println("Scan PICC to see UID and type..."); 
}


/*
@brief check if all clips are properly closed
*/
bool isClipClosed() {

  return true;
}

/*
@brief validates RFID chip
@return true if correct
*/
bool validateRFID() {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  // Dump debug info about the card. PICC_HaltA() is automatically called.
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}

void turnOnLight() {
  digitalWrite(13, HIGH);    // set the LED off
}

void turnOffLight() {
  digitalWrite(13, LOW);    // set the LED off
}

void loop() {

  //after state change
  if (state != previous_state) {
    if(state == STATE_OK) {
      turnOffLight();
    }
    else if(state == STATE_OPENED) {
      turnOnLight();
    }
    else (state == STATE_CLOSED) {
      turnOnLight();
    }
  }

  digitalWrite(13, HIGH);   // set the LED on
  delay(1000);              // wait for a second
  
  delay(1000);              // wait for a second
  Serial.write("Blinked");



  //state machine
  previous_state = state;
  if(state == STATE_OK) {
    if( !isClipClosed() ) {
      state = STATE_OPENED;
      return;
    }

  }
  else if(state == STATE_CLOSED) {
    if( !isClipClosed() ) {
      state = STATE_OPENED;
      return;
    }
    else if( validateRFID() ){
      state = STATE_OK;
      return;
    }
  }
  //opened state
  else {
    if( isClipClosed() ) {
      state = STATE_CLOSED;
      return;
    }
    else return;
  }
  

}


