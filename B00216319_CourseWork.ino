#include <ESP8266WiFi.h>                // WiFi Library
#include <SimpleDHT.h>                  // Digital Temp Library

const char* ssid = "";       // This is the SSID of the network; "BTHub4-NKRC";
const char* password = "";    // This is the password for the network; "c272e73d5b";
WiFiServer server(80);                  // This is the service port being used

int pinTemp1 = 16;
SimpleDHT11 dht11;

void setup() {
  Serial.begin(115200);                 
  delay(20);                            // Delay in miliseconds between reads

  /**
   * Connecting to the WiFi network
   * Appropriate messages displayed
   **/

   Serial.println();
   Serial.print("Connect to: ");
   Serial.println(ssid);
   WiFi.begin(ssid, password);          // Parameters passed to authenticate to the network

   while (WiFi.status() != WL_CONNECTED){ // while loop to display connection whileunconnected to WiFi network
    delay(500);
    Serial.println("establishing connection...");    
   }

   Serial.println("");
   Serial.print("Connected to: ");
   Serial.println(ssid);

   /**
    * Start the server command
    **/

    server.begin();
    Serial.println("WeMos D1 Server started");
    Serial.print("Allocation of IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("");

}

void loop() {
  
  //Serial.println("Commence Temperature Readings");
  
  byte temp1 = 0;
  byte humidity = 0;
  byte data[40] = {0};
  
  if (dht11.read(pinTemp1, &temp1, &humidity, data)) {
    Serial.print("Read DHTll failed");
    return;
  }

 /* Serial.print("Temp 1: ");
  for (int i=0; i<40; i++){
    Serial.print((int)data[i]);
    if (i>0 && ((i+1) % 4) ==0) {
      Serial.print(' ');
    }
  }*/
  Serial.println("");
  Serial.print("Temparature 1: ");
  Serial.print((int)temp1); Serial.print(" *C, ");
  delay(3000);

  

}
