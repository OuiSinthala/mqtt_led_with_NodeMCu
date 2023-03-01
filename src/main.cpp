#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WIFI information
const char* ssid = "wifiname";
const char* password = "wifipas";

// MQTT broker information
const char* mqtt_server = "serverip"; // broker.hivemq.com or broker.mqttdashboard.com
const uint16_t mqtt_port = 1883;
const char* mqtt_topic = "iot/workshop"; // topic name (can be anything)

// Creating objects
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// function prototypes
void wifiConnection();
void mqttReconnection();
void callback(char* topic, byte* payload, int length);

//pins
const int ledPin = D0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  wifiConnection();
  // connect to mqtt broker(server)
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback); // recieve message from the broker
}

void loop() {
  // handle disconnection
  if (!mqttClient.connected()) {
    mqttReconnection();
  }
  mqttClient.loop(); //let the board keeps communicate with the broker everytime.
}

void wifiConnection() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqttReconnection() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT broker...");
    String clientId = "esp8266-" + WiFi.macAddress();
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
      mqttClient.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

// this function enable the NodeMCU board to be able to recieve data from the broker
void callback(char* topic, byte* payload, int length) {
  Serial.print("Message received on topic [");
  Serial.print(topic);
  Serial.print("]: ");
  String message = "";
  for (int i = 0; i < length; i++) {
    // Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println(message);

  if (message == "IoTWorkshop"){
    digitalWrite(ledPin, 1);
  }else if (message == "off"){
    digitalWrite(ledPin, 0);
  }
}
