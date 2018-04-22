#include <ESP8266WiFi.h>                // WiFi Library
#include <SimpleDHT.h>                  // Digital Temp Library
#include <SPI.h>                        // SPI Bus Library
#include <MFRC522.h>                    // Library
#include <PubSubClient.h>               // MQTT Library
#include <Wire.h>

// WiFi Network and Server Details
const char* ssid = "BTHub4-NKRC";         // This is the SSID of the network; "BTHub4-NKRC";
const char* password = "c272e73d5b";      // This is the password for the network; "c272e73d5b";
const char* mqtt_server = "192.168.1.95"; // IP address for the node-red broker
WiFiServer server(80);                    // This is the service port being used
WiFiClient espClient;
PubSubClient client(espClient);

// LED Pin Details
int ledPin = 14;                          // LED Pin - (5 Uno)
int buttonApin = 0;                       // LED Pin - (9 Uno)
int buttonBpin = 13;                      // LED Pin - (8 Uno)
byte leds = 0;

// Temp Pin Details
int pinTemp1 = 16;
int pinTemp2 = 5;
SimpleDHT11 dht11;
SimpleDHT11 dht112;

// RFID Pin Details
#define RST_PIN   2                       // RST Pin 2 - (9 Uno)
#define SS_PIN   15                       // SS Pin 15 - (10 Uno)
MFRC522 mfrc522(SS_PIN, RST_PIN);         // Create MFRC522 instance
#define NEW_UID {0xDE, 0xAD, 0xBE, 0xEF}
MFRC522::MIFARE_Key key;




void setup_wifi() {
   Serial.println();
   Serial.print("Connect to: ");
   Serial.println(ssid);
   WiFi.begin(ssid, password);                      // Parameters passed to authenticate to the network

   while (WiFi.status() != WL_CONNECTED){           // while loop to display connection while unconnected to WiFi network
    delay(500);
    Serial.println("establishing WiFi connection...");    
   }
   Serial.println("");
   Serial.print("Connected to: ");
   Serial.println(ssid);

   //Start the server command
    server.begin();
    Serial.println("WeMos D1 Server started");
    Serial.print("Allocation of IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("");
  
}

/*void setup_mqtt() {
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
  
}*/

void read_temps(){

  byte temp1 = 0;
  byte humidity = 0;
  byte data[40] = {0};
  
  if (dht11.read(pinTemp1, &temp1, &humidity, data)) {
    Serial.println("Read DHTll failed");
    return;
  }

  Serial.println("");
  Serial.print("Equipment Room: ");
  Serial.print((int)temp1); 
  Serial.print(" *C, ");
  Serial.println("");
  delay(3000);
  client.publish("T1", "Hello World" );


  byte temp2 = 0;
  byte humidity2 = 0;
  byte data2[40] = {0};

    if (dht112.read(pinTemp2, &temp2, &humidity2, data2)) {
    Serial.println("Read DHTll2 failed");
    return;
  }

  Serial.print("Growing Area: ");
  Serial.print((int)temp2); Serial.print(" *C, ");
  delay(3000);
  client.publish("T1", "Temp 2");
    
  }

  void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("establishing IoT Broker connection over MQTT...");
 // Attempt to connect
 if (client.connect("ESP8266 Client")) {
  Serial.println("connected");
  // ... and subscribe to topic
  client.subscribe("T1");
 } else {
  Serial.print("failed, rc=");
  Serial.print(client.state());
  Serial.println(" try again in 5 seconds");
  // Wait 5 seconds before retrying
  delay(5000);
  }
 }
}


void setup() {
  Serial.begin(115200);
  delay(20);                                      // Delay in miliseconds between reads
  setup_wifi();
  //setup_mqtt();
  client.setServer(mqtt_server, 1883);
  SPI.begin();                                    // Start Init SPI bus
  mfrc522.PCD_Init();                             // Init MFRC522 card
  pinMode(ledPin, OUTPUT);
  pinMode(buttonApin, INPUT_PULLUP);  
  pinMode(buttonBpin, INPUT_PULLUP);


}

void loop() {
  
  if (!client.connected()) {
  reconnect();
 }
 read_temps();
 client.loop();
 

}
