#include <ESP8266WiFi.h>                // WiFi Library
#include <SimpleDHT.h>                  // Digital Temp Library
#include <SPI.h>                        // SPI Bus Library
#include <MFRC522.h>                    // RFID Library
#include <PubSubClient.h>               // MQTT Library


// WiFi Network and Server Details
const char* ssid = "Nicky";         // This is the SSID of the network;
const char* password = "youknowit";      // This is the password for the network;
const char* mqtt_server = "192.168.43.118"; // IP address for the node-red broker
WiFiServer server(80);                    // This is the service port being used
WiFiClient espClient;
PubSubClient client(espClient);

// LED Pin Details
#define ledPinCooler 3                      // LED Pin - (5 Uno)
#define ledPinHeater 1                      // LED Pin - (6 Uno)
#define ledPinNormal 16                      // LED Pin - (7 Uno)
byte leds = 0;

// Temp Pin Details
int pinTemp1 = 5;                         // Temp 1 Pin
int pinTemp2 = 4;                         // Temp 2 Pin
SimpleDHT11 dht11;
SimpleDHT11 dht112;

// RFID Pin Details
#define RST_PIN   2                       // RST Pin 2 - (9 Uno)
#define SS_PIN   15                       // SS Pin 15 - (10 Uno)
MFRC522 mfrc522(SS_PIN, RST_PIN);         // Create MFRC522 instance
#define NEW_UID {0xDE, 0xAD, 0xBE, 0xEF}
MFRC522::MIFARE_Key key;


/**
 * WiFi Function
 * Connects to a WiFi Network
 * using the SSID and Password
 * as parameters
 */

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


/**
 * This function reads the 2 temps
 * converts to chars and passes the messages
 * by MQTT. This function also determines
 * if there has been a change in Temp and lights
 * The appropriate LED
 */
void read_temps(){

  byte temp1 = 0;
  byte humidity = 0;
  byte data[40] = {0};
  
  if (dht11.read(pinTemp1, &temp1, &humidity, data)) {
    Serial.println("Read DHTll failed");
    return;
  }

  Serial.println("");
  Serial.print("Equipment Room Temp: ");
  Serial.print((int)temp1); Serial.print(" *C, ");              // Print out temp to serial monitor
  Serial.println("");
  char buffer1[10];
  const char* mqttTemp1;
  double equipRoom = temp1;
  mqttTemp1 = dtostrf(equipRoom,4,2,buffer1);                  // Convert temp to const char for MQTT message
  client.publish("TempEquipRoom", mqttTemp1);                  // Publish message using MQTT to broker queue
  delay(1000);


  byte temp2 = 0;
  byte humidity2 = 0;
  byte data2[40] = {0};

    if (dht112.read(pinTemp2, &temp2, &humidity2, data2)) {
    Serial.println("Read DHTll2 failed");
    return;
  }

  Serial.print("Growing Area Temp: ");
  Serial.print((int)temp2); Serial.println(" *C, ");            // Print out temp to serial monitor
  Serial.print("Growing Area Humidity: ");
  Serial.print((int)humidity); Serial.println(" %");            // Print out humidity to serial monitor
  
  char buffer2[10];
  const char* mqttTemp2;
  double growingArea = temp2;
  mqttTemp2 = dtostrf(growingArea,4,2,buffer2);              // Convert temp to const char for MQTT message
  client.publish("TempGrowArea", mqttTemp2);                 // Publish message using MQTT to broker queue
  delay(1000);

  char buffer3[10];
  const char* mqttHumid;
  double growingAreaHumid = humidity;
  mqttHumid = dtostrf(growingAreaHumid,2,0,buffer3);              // Convert humidity to const char for MQTT message
  client.publish("HumGrowArea", mqttHumid);                 // Publish message using MQTT to broker queue
  delay(1000);


    if ((int)temp1+2 < (int)temp2)
  {
    digitalWrite(ledPinCooler, LOW);
    digitalWrite(ledPinHeater, HIGH);
    digitalWrite(ledPinNormal, LOW);
    client.publish("HeatCoolStatus", "Heater On");
  }
  else if ((int)temp1 > (int)temp2+2)
  {
    digitalWrite(ledPinCooler, HIGH);
    digitalWrite(ledPinHeater, LOW);
    digitalWrite(ledPinNormal, LOW);
    client.publish("HeatCoolStatus", "Cooler On");
  }
  else{
    digitalWrite(ledPinCooler, LOW);
    digitalWrite(ledPinHeater, LOW);
    digitalWrite(ledPinNormal, HIGH);
    client.publish("HeatCoolStatus", "Stable");    
  }
  
  }

  /**
   * This function checks if a card or fob
   * is presented. The unique code generates
   * a String which is then checked to see if valid
   * for entry. The name is then passed to the broker
   * by MQTT message.
   */

  void read_door(){
    if ( ! mfrc522.PICC_IsNewCardPresent()){
    return;
   }
   if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
   }
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
if (content.substring(1) == "8B 88 DB D9")
  {
    Serial.println("Authorized access");
    Serial.println();
    client.publish("DoorSecurity", "Technician");
    delay(1000);
    
  }
  
  else if (content.substring(1) == "F7 A3 0A 0C")
  {
    Serial.println("Authorized access");
    Serial.println();
    client.publish("DoorSecurity", "Engineer");
    delay(1000);
    
  }
 
 else   {
    Serial.println(" Access denied");
    delay(1000);
  }
  }


/**
 * This function is called when a message is received
 * by the IoT Broker. It then checks what the message is
 * and controls the on/off of the appropriate LED.
 */
 void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];
  Serial.print(receivedChar);
  
  if (receivedChar == '1')
  digitalWrite(ledPinCooler, HIGH);
  else if (receivedChar == '0')
   digitalWrite(ledPinCooler, LOW);
   else if (receivedChar == '2')
   digitalWrite(ledPinHeater, HIGH);
   else if (receivedChar == '3')
   digitalWrite(ledPinHeater, LOW); 

  }

}

/**
 * This function loops until a connection
 * is made with the IoT broker. Once connected
 * it subscribes to topics
 */

  void reconnect() {
 while (!client.connected()) {
 Serial.print("establishing IoT Broker connection over MQTT...");
 if (client.connect("ESP8266 Client")) {
  Serial.println("connected");
  client.subscribe("CoolerOnOff");
  client.subscribe("HeaterOnOff");
 } else {
  Serial.print("failed, rc=");
  Serial.print(client.state());
  Serial.println(" try again in 5 seconds");
  delay(5000);
  }
 }
}

/**
 * Core function at set up which
 * starts functions and calls created functions such
 * as setup_wifi().
 */

void setup() {
  Serial.begin(115200);
  delay(20);                                      // Delay in miliseconds between reads
  SPI.begin();                                    // Start Init SPI bus
  mfrc522.PCD_Init();                             // Init MFRC522 card
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(ledPinCooler, OUTPUT);                // Sets the outout of the pin
  pinMode(ledPinHeater, OUTPUT);                // Sets the outout of the pin
  pinMode(ledPinNormal, OUTPUT);                // Sets the outout of the pin

}

/**
 * Core loop function which continually loops
 * and calls the temp and RFID functions. It also
 * ensure the connection to the broker is set up.
 */

void loop() {  
 if (!client.connected()) {
  reconnect();}
 read_door();
 read_temps();
 client.loop();
}
