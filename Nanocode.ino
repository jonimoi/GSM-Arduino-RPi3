/*

This program recieves textmessages from the A7 GSM/GPRS/GPS module.
If the word 'bild' is recieved, the radiomodule passes it to the 
reading nrf module.

This code is tested and compiled on a Arduino Nano/Uno.

Made by: JonMoi
*/

#include <SPI.h>        // Used for SPI communication
#include <RF24.h>       // Used for Radiomodules
#include "SIM900.h"     // Used for reading SIMcards/messages
#include "sms.h"        // Needed to store textmessages
#include "nRF24L01.h"   // Radiomodulelibrary
#include "RF24.h"       // Radiomodulelibrary

SMSGSM sms;
boolean started=false;
char smsbuffer[160];              
char n[20];                       // Stores 20 textmessages(in this case not really needed)
char sms_position;                // Saves position of textmessage(Not really needed)
char phone_number[20];            // array for the phone number string
char sms_text[100];               // Max lenght of a textmessage
char Str[5] = "bild";             // Array to store the trigger of RPi taking a picture
char Test[5] = "test";            // Array to store the trigger for testing Nano functionality
const int LED_OK = 6;             // Green LED for OK
const int LED_NOK = 5;            // Red led for not OK
RF24 radio(9, 10);                // Radio initialization, CE CSN pins
// Test & blinkfunctions
void testSMSfunctionWithLeds();
void radioBlink();

// Setup code
void setup() {
  // Radio initialization
  radio.begin();                         // Start radio
  radio.setPALevel(RF24_PA_MAX);         // Set to max for longer distancecommunication, use LOW if modules are close to eachother
  radio.setChannel(0x76);
  radio.openWritingPipe(0xF0F0F0F0E1LL); // Radio pipe for communicating with the RPi with NRF24L01
  radio.enableDynamicPayloads();
  radio.powerUp();
  // For serial debugging, A7 needs to be set to a baudrate of 9600(factorystandard: 115200)
  Serial.begin(9600);
  pinMode(LED_OK, OUTPUT);               
  pinMode(LED_NOK, OUTPUT);
  digitalWrite(LED_NOK, HIGH); // Red led on to show that setupprocess still is ongoing
  Serial.println("GSM-module starting..");
    if (gsm.begin(9600)) 
    {
        Serial.println("\nstatus=Ready!");
        started=true;
    } 
    else 
        Serial.println("\nstatus=Waiting...");
    if(started) 
    {    
       for(int i=1;i<=20;i++)
       {
           sms.DeleteSMS(i);  // Function to delete all previous messages
       }
    }
}
/*
 * If setup is complete the loop begins to listen to textmessages recieved by the A7-module.
 * It only writes to radio if a certain message is recieved, in this case 'test' or 'bild'
 */
void loop() {
 if(started) 
    {
        digitalWrite(LED_NOK, LOW); // Not OK led turns off and OK led is turned on to show user that everything is up to speed.
        digitalWrite(LED_OK, HIGH);
        sms_position=sms.IsSMSPresent(SMS_UNREAD);
        if (sms_position) 
        {
            sms.GetSMS(sms_position, phone_number, sms_text, 100);  // The current library in use can store numbers, texts in inbox and so forth, but in this case
                                                                    // we are just interested in what the latest textmessage says.
              if (strcmp(sms_text,Str) == 0) {                      // If the textmessage is equal to text saved in Str-Array, something happens. 
                 radio.write(&sms_text, sizeof(sms_text));          // Radio writes the recieved message in sms_text and passes it through the NRF-module.
                 radioBlink();                                      // Blinks leds to show that it was trying to send to RPi & prints in serialmonitor
            }
            else if (strcmp(sms_text,Test) == 0) {                  // If textmessage is equal to text saved in Test-array, testfunction is called
            testSMSfunctionWithLeds();              
            }
            else {
              Serial.println("SMS received, no further action.");     // If message without action is recieved
            }           
        }   
        delay(1000);       
    }
}
// Serial / LED function for testing if A7 and Nano is working
void testSMSfunctionWithLeds() {
              Serial.println("Message received, blinking LED's two times..");
              digitalWrite(LED_OK, LOW);
              delay(300);
              digitalWrite(LED_NOK, HIGH);
              digitalWrite(LED_OK, HIGH);
              delay(300);
              digitalWrite(LED_NOK, LOW);
              digitalWrite(LED_OK, LOW);
              delay(300);
              digitalWrite(LED_NOK, HIGH);
              digitalWrite(LED_OK, HIGH);
              delay(300);
              digitalWrite(LED_NOK, LOW);
              digitalWrite(LED_OK, LOW);
              delay(300);
              digitalWrite(LED_OK, HIGH);
}
// Serial / LED confirmation that a msg is recieved and sent
void radioBlink(){
              Serial.println("Sending the message to slave...");
              digitalWrite(LED_OK, LOW);
              delay(200);
              digitalWrite(LED_OK, HIGH);
              delay(200);
              digitalWrite(LED_OK, LOW);
              delay(200);
              digitalWrite(LED_OK, HIGH);
}
