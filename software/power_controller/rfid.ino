#ifdef RFID_SERIAL

	#include <SoftwareSerial.h>
	#define RFID_ID_LENGTH 10 + 2
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
    rfid_serial.setTimeout(10);
    //turn on rfid
    pinMode(RFID_NOT_ENABLE, OUTPUT);
    digitalWrite(RFID_NOT_ENABLE, LOW);
#elif defined RFID_SPI
    SPI.begin();			// Init SPI bus
    mfrc522.PCD_Init();		// Init MFRC522
#endif
}

String read_rfid()
{
    String rfid = "";

#ifdef RFID_SERIAL
    if(rfid_serial.available() == RFID_ID_LENGTH)
    {
        for(int i = 0; i < RFID_ID_LENGTH; i++)
        {
            rfid.concat((char)rfid_serial.read());
        }
        //trim newline
        rfid.trim();
        Serial.print("got RFID: "); Serial.println(rfid);
        rfid_serial.flush();
    }
    else if(rfid_serial.available() > RFID_ID_LENGTH)
        rfid_serial.flush();

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
        Serial.print("got RFID: "); Serial.println(rfid);
#endif

	return rfid;
}
