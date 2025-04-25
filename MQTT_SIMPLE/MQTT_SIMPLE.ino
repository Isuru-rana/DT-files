#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// MQTT settings
const char* mqtt_server = "www.example.com";
const int mqtt_port = 1883;
const char* mqtt_user = "mqtt_username";
const char* mqtt_pass = "mqtt_password";
const char* mqtt_client_id = "ESP32Client";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000;  // 5 seconds between reconnect attempts
unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 10000;   // 10 seconds between MQTT publishes


#define LED_PIN 2 

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  setupWiFi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callBack);
}

void setupWiFi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Set hostname (optional)
  String hostname = "ESP32-Device";
  WiFi.setHostname(hostname.c_str());

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("WiFi connection FAILED");
    ESP.restart();
  }
}

void reconnect() {
  // Check if we're already connected
  if (client.connected()) {
    return;
  }
  
  if (millis() - lastReconnectAttempt > reconnectInterval) {
    lastReconnectAttempt = millis();
    
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      MQTTsubscribe();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again later");
    }
  }
}


void checkWiFiConnection() {
  // If WiFi is disconnected, attempt to reconnect
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    setupWiFi();
  }
}

void loop() {
  // Check WiFi connection periodically
  checkWiFiConnection();
  
  // Handle MQTT connection
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Publish data periodically
  publishData();
  
  // Add any other periodic tasks here
  
  // Small delay to prevent CPU hogging
  delay(100);
}