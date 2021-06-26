#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

const int led = 3;           // the PWM pin the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 2;    // how many points to fade the LED by
const int addBrightnessAmount = 50;
int brightnessAmount;

bool gameOver = false;
int totalTaggedAmount;
const int tagPointAmount = 1;
String tagBase[5];


void setup() 
{
    pinMode(led, OUTPUT);
    Serial.begin(9600);   // Initiate a serial communication
    SPI.begin();      // Initiate  SPI bus
    mfrc522.PCD_Init();   // Initiate MFRC522
    Serial.println("Approximate your card to the reader...");
    Serial.println();
}

void loop() 
{
    while (gameOver == false)
    {
        // Look for new cards
        if ( ! mfrc522.PICC_IsNewCardPresent()) 
        {
          return;
        }
        // Select one of the cards
        if ( ! mfrc522.PICC_ReadCardSerial()) 
        {
          return;
        }

        //Show UID on serial monitor
        Serial.print("UID tag :");
        String content= "";
        byte letter;
        for (byte i = 0; i < mfrc522.uid.size; i++) 
        {
            Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
            Serial.print(mfrc522.uid.uidByte[i], HEX);
            content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
            content.concat(String(mfrc522.uid.uidByte[i], HEX));
        }
        Serial.println();
        Serial.print("Message : ");
        content.toUpperCase();

        // IMPORTANT --> content.substring treba u for petlju da ide i da se proverava da li je tag poznat i da li je tag vec iskoriscen

        if (content.substring(1) == "3A 98 E2 3F") //change here the UID of the card/cards that you want to give access
        {
            Serial.println("Authorized access");
            Serial.println();
            accessGrantedLed();
            totalTaggedAmount += tagPointAmount;

            if (totalTaggedAmount == 5)
            {
                Serial.println("Game Over");
                gameOver = true;
                brightness = brightnessAmount;
            }
            //Serial.println(totalTaggedAmount);
            delay(3000);
        }
        else {
            Serial.println(" Access denied");
            accessDeniedLed();
            delay(3000);
        }
    }
    fadeEffect();
}

void accessDeniedLed() 
{
    analogWrite(led, 255);
    totalTaggedAmount = 0;
    brightnessAmount = 0;
    delay(500);
    analogWrite(led, 0);
}

void accessGrantedLed() 
{
    brightnessAmount = brightnessAmount + addBrightnessAmount;
    //Serial.println(brightnessAmount);
    analogWrite(led, brightnessAmount);
}

void fadeEffect() 
{
    analogWrite(led, brightness);
    brightness = brightness + fadeAmount;
    if (brightness <= 1 || brightness >= 254) {
      fadeAmount = -fadeAmount;
    }
    delay(30);
}