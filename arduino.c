/*
Team Safety Lab for PPE enforcement

*/

#include <SPI.h>
#include <MFRC522.h>


// RFID reader config
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

#define STATE_OPENED 0
#define STATE_CLOSED 1
#define STATE_OK 2


int state = STATE_OPENED;
int previous_state = -1;

#define NUM_STORED_CARDS 1

byte storedCard[NUM_STORED_CARDS][4];   // Stores an ID read from EEPROM
byte readCard[4];   // Stores scanned ID read from RFID Module
byte masterCard[4];   // Stores master card's ID read from EEPROM


//card=0xB635278D
//rfid =0xF0FF736


void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(8, OUTPUT);   
 
  Serial.begin(9600);	// Initialize serial communications with the PC
  SPI.begin();			// Init SPI bus
  mfrc522.PCD_Init();	// Init MFRC522 card
  Serial.println("Scan PICC to see UID and type..."); 

  storedCard[0][0] = 0xB6;
  storedCard[0][1] = 0x35;
  storedCard[0][2] = 0x27;
  storedCard[0][3] = 0x8D;
} 


/*
@brief check if all clips are properly closed
*/
bool isClipClosed() {
  //TODO check the switch from a clip
  return true;
}


bool compareTwoCards ( byte a[], byte b[] ) {
  if ( a[0] != 0 )      // Make sure there is something in the array first
  for ( uint8_t k = 0; k < 4; k++ ) {   // Loop 4 times
    if ( a[k] != b[k] )     // IF a != b then set match = false, one fails, all fail
      return false;
  }
  return true;
}


/*
@brief reads RFID

*/

bool getID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return false;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  Serial.println(F("Scanned PICC's UID:"));
  for ( uint8_t i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
   Serial.println("");

  mfrc522.PICC_HaltA(); // Stop reading
   return true;

 
}


void sendRFID() {
  //TODO
}

/*
@brief validates RFID chip
@return true if correct
*/
bool validateRFID() {
  // Look for new cards
  if( !getID()) return false;

  //TODO write procedure to check that RFID is known
  for(int i = 0; i < NUM_STORED_CARDS; i++) {
    if(compareTwoCards(readCard, storedCard[i])) {
      return true;
      Serial.println("Known card."); 
    }
  }
  Serial.println("Unknown card."); 
  return false;
  

}


void turnOnLight() {
  digitalWrite(8, HIGH);    // set the LED off
}

void turnOffLight() {
  digitalWrite(8, LOW);    // set the LED off
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
    else if(state == STATE_CLOSED) {
      turnOnLight();
    }
  }


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


