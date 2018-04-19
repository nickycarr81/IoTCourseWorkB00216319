#include <ESP8266WiFi.h>                // WiFi Library
#include <SimpleDHT.h>                  // Digital Temp Library
#include <SPI.h>                        // SPI Bus Library
#include <MFRC522.h>                    // Library
#include <PubSubClient.h>               // MQTT Library

const char* ssid = "BTHub4-NKRC";       // This is the SSID of the network; "BTHub4-NKRC";
const char* password = "c272e73d5b";    // This is the password for the network; "c272e73d5b";
WiFiServer server(80);                  // This is the service port being used
const char* mqtt_server = "192.168.1.95";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;



int ledPin = 14;    // LED Pin - (5 Uno)
int buttonApin = 0;   // LED Pin - (9 Uno)
int buttonBpin = 13;   // LED Pin - (8 Uno)
byte leds = 0;

int pinTemp1 = 16;
int pinTemp2 = 5;
SimpleDHT11 dht11;
SimpleDHT11 dht112;


#define RST_PIN   2     // RST Pin 2 - (9 Uno)
#define SS_PIN   15     // SS Pin 15 - (10 Uno)
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
#define NEW_UID {0xDE, 0xAD, 0xBE, 0xEF}
MFRC522::MIFARE_Key key;



void setup() {
  Serial.begin(115200);                 
  delay(20);                            // Delay in miliseconds between reads
  SPI.begin();                          // Start Init SPI bus
  mfrc522.PCD_Init();                   // Init MFRC522 card

    pinMode(ledPin, OUTPUT);
    pinMode(buttonApin, INPUT_PULLUP);  
    pinMode(buttonBpin, INPUT_PULLUP);

  /**
   * Connecting to the WiFi network
   * Appropriate messages displayed
   **/

   Serial.println();
   Serial.print("Connect to: ");
   Serial.println(ssid);
   WiFi.begin(ssid, password);          // Parameters passed to authenticate to the network

   while (WiFi.status() != WL_CONNECTED){ // while loop to display connection while unconnected to WiFi network
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



    /************************************************************************************************
     ******************************************** MQTT***********************************************
     ************************************************************************************************/

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP8266Client")) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish("outTopic", "hello world");
            // ... and resubscribe
            client.subscribe("inTopic");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}



      /************************************************************************************************
     ******************************************** MQTT***********************************************
     ************************************************************************************************/

}

void loop() {



  //Serial.println("Commence Temperature Readings");
  
  byte temp1 = 0;
  byte humidity = 0;
  byte data[40] = {0};
  
  if (dht11.read(pinTemp1, &temp1, &humidity, data)) {
    Serial.println("Read DHTll failed");
    return;
  }


  Serial.println("");
  Serial.print("Equipment Room: ");
  Serial.print((int)temp1); Serial.print(" *C, ");
  Serial.println("");
  delay(3000);


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

  if ((int)temp1 == (int)temp2)
  {
    digitalWrite(ledPin, LOW);
  }
  if ((int)temp1 > (int)temp2)
  {
    digitalWrite(ledPin, HIGH);
  }

      // Look for new cards, and select one if present
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }

    Serial.println("");
    Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  } 

  delay(2000);


      if (!client.connected()) {
        reconnect();
    }
    client.loop();

    long now = millis();
    if (now - lastMsg > 2000) {
        lastMsg = now;
        ++value;
        snprintf (msg, 75, "hello world #%ld", value);
        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish("outTopic", msg);
    }





}
