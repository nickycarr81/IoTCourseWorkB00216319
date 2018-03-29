
#include <ESP8266WiFi.h>  // Wi-Fi
#include <SimpleDHT.h>    // Digital Temp

const char* ssid = "Nicky";//type your ssid
const char* password = "youknowit";//type your password
WiFiServer server(80);

/**
 * LED
 * Variables
 **/

int ledPin = 14;
int buttonApin = 2;
int buttonBpin = 0;

/**
 * Digital Temp
 * Variables
 **/
int pinDHT11 = 16;
SimpleDHT11 dht11;



void setup() {

  pinMode(ledPin, OUTPUT);
  pinMode(buttonApin, INPUT_PULLUP);  
  pinMode(buttonBpin, INPUT_PULLUP);

}

void loop() {

  if (digitalRead(buttonApin) == LOW)
  {
    digitalWrite(ledPin, HIGH);
  }
  if (digitalRead(buttonBpin) == LOW)
  {
    digitalWrite(ledPin, LOW);
  }

}
