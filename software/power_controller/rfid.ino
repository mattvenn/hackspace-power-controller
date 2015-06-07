#ifdef RFID_SERIAL

	#include <SoftwareSerial.h>
	#define RFID_ID_LENGTH 10
	SoftwareSerial rfid_serial(RFID_RX, RFID_TX); // RX, TX

#elif defined RFID_SPI

	#include <SPI.h>
	#include <MFRC522.h>
	MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance

#endif


void setup_rfid()
{
#ifdef RFID_SERIAL
    rfid_serial.begin(2400);
    rfid_serial.setTimeout(50);
    pinMode(RFID_NOT_ENABLE, OUTPUT);
    digitalWrite(RFID_NOT_ENABLE, HIGH);
#elif defined RFID_SPI
    SPI.begin();			// Init SPI bus
    mfrc522.PCD_Init();		// Init MFRC522
#endif
}

void start_rfid()
{
#ifdef RFID_SERIAL
    //this makes sure that there are no old ids in the serial buffer
    digitalWrite(RFID_NOT_ENABLE, LOW);
    rfid_serial.flush();
#endif
}

String read_rfid()
{
    String rfid = "";

#ifdef RFID_SERIAL

    if(rfid_serial.available())
    {
        Serial.println("rfid serial available");
        //get rid of any leading chars
        rfid_serial.readStringUntil('\n');
        //read till the end of the ID
        rfid = rfid_serial.readStringUntil('\r');
        //check we've got the right length
        if(rfid.length() == RFID_ID_LENGTH)
            //yes; then turn off reader
            digitalWrite(RFID_NOT_ENABLE, HIGH);
        else
            //no; wipe it.
            rfid = "";
    }

#elif defined RFID_SPI
	// Look for new cards
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return rfid;
	}
	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return rfid;
	}
	
        mfrc522.PICC_HaltA();
	for (byte i = 0; i < mfrc522.uid.size; i++) 
	{
		if(mfrc522.uid.uidByte[i] < 0x10)
		rfid.concat("0");
		rfid.concat(String(mfrc522.uid.uidByte[i], HEX));
	} 
#endif

	return rfid;
}
