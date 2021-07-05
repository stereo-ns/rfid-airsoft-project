//Libraries needed for project, for connecting and making RFID-RC522 module work
#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance for RFID reader

const int ledGreen = 6; // the PWM pin the LED Green is attached to
const int ledRed = 7; // the pin the LED Red is attached to
const int ledBlue = 4; // the pin the LED Blue is attached to
const int led = 3;      // the PWM pin the LED is attached to
const int button = 2; // button pin
const int ledButton = 13; // control LED for button (HIGH and LOW)
int brightness = 0;     // how bright the LED is, both LEDs on pin 3 and pin 6
int fadeAmount = 2;     // how many points to fade the LED by
const int addBrightnessAmount = 50; // used when known player is tagged
int brightnessAmount; // initial amount for LED, is OFF at beginning

bool gameOver = false; // when all friendly players are tagged this will be TRUE
int totalTaggedAmount; // amount of friendly players tagged
const int tagPointAmount = 1; // to be considered for diferent type of calculating points for win

// needs to be implemented later, base check for comparing friendly tags


void setup() 
{
    pinMode(led, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(ledBlue, OUTPUT);

    pinMode(ledButton, OUTPUT);
    pinMode(button, INPUT);
    
    Serial.begin(9600);   // Initiate a serial communication
    SPI.begin();      // Initiate  SPI bus
    mfrc522.PCD_Init();   // Initiate MFRC522
    Serial.println("Approximate your card to the reader...");
    Serial.println();
}

void loop() 
{
    if (digitalRead(button) == LOW) {
      //digitalWrite(ledButton, HIGH);
      if (gameOver == true)
      {
        fadeEffect();
      }
    } else {
      //digitalWrite(ledButton, LOW);
      resetAll();    
    }
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
            digitalWrite(ledBlue, HIGH);
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
            digitalWrite(ledBlue, LOW);
        }
        else {
            Serial.println(" Access denied");
            accessDeniedLed();
            delay(3000);
        }
    }
}

void accessDeniedLed() 
{
    analogWrite(led, 255);
    digitalWrite(ledRed, HIGH);
    totalTaggedAmount = 0;
    brightnessAmount = 0;
    delay(3000);
    analogWrite(led, 0);
    digitalWrite(ledRed, LOW);
}

void accessGrantedLed() 
{
    brightnessAmount = brightnessAmount + addBrightnessAmount;
    analogWrite(led, brightnessAmount);
}

void fadeEffect() 
{
    analogWrite(led, brightness);
    analogWrite(ledGreen, brightness);
    brightness = brightness + fadeAmount;
    if (brightness <= 1 || brightness >= 254) {
      fadeAmount = -fadeAmount;
    }
    delay(30);
}

void resetAll()
{
  totalTaggedAmount = 0;
  brightnessAmount = 0;
  brightness = 0;
  analogWrite(led, brightness);
  analogWrite(ledGreen, brightness);
  gameOver = false;
  Serial.println("RESET");
  delay(3000);
}