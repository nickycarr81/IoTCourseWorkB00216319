#include <ESP8266WiFi.h>

const char* ssid = "**********";       // This is the SSID of the network;
const char* password = "********";    // This is the password for the network;

WiFiServer server(80);                  // This is the service port being used

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
   

}

void loop() {
  // put your main code here, to run repeatedly:

}
