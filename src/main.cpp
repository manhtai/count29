#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>         //https://github.com/knolleary/pubsubclient

//for LED status
#include <Ticker.h>
Ticker ticker;

// D pins, D0 & D4 are 2 built-in LEDs
int D[10] = {D0, D1, D2, D3, D4, D5, D6, D7, D8, D9};

// MQTT
// TODO: Replace this with your server
const char* mqttServer = "test.mosquitto.org";
const char* mqttUsername = NULL;
const char* mqttPassword = NULL;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[100];
char tmpMsg[10];

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on/off the D pin if an D was received as first character
  if (length < 2 || (char)payload[0] != 'D')
    return;

  int led = (char)payload[1] - '0';
  int state = digitalRead(D[led]);
  digitalWrite(D[led], !state);
}

void mqttReconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqttUsername, mqttPassword)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "Connected!");
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

void tick() {
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

void wifiSetup() {
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  if (!wifiManager.autoConnect("My-ESP-rocks")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  ticker.detach();
  //turn LED off
  digitalWrite(BUILTIN_LED, HIGH);
}

void mqttSetup() {
  client.setServer(mqttServer, 1883);
  client.setCallback(mqttCallback);  
}

void ledSetup() {
  for (int i = 0; i < 10; i++) {
      pinMode(D[i], OUTPUT);
  }
}


void setup() {
  Serial.begin(115200);
  ledSetup();
  wifiSetup();
  mqttSetup();
}

void loop() {
  if (!client.connected()) {
    mqttReconnect();
  }
  client.loop();
  long now = millis();
  
  if (now - lastMsg > 2000) {
    lastMsg = now;
    strcpy(msg, "");
    for (int i = 0; i < 10; i++) {
        sprintf(tmpMsg, "D%i:%i ", i, digitalRead(D[i]));
        strcat(msg, tmpMsg);
    }
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }
}
