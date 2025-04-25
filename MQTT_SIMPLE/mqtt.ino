void MQTTsubscribe() {
  // Subscribe to your MQTT topics here
  client.subscribe("esp32/led");

  // Add additional topics as needed
  // client.subscribe("home/livingroom/temperature");
  // client.subscribe("home/garden/humidity");
}

void callBack(char* topic, byte* payload, unsigned int length) {
  // This line indicates the receipt of a message
  Serial.println("Message arrived in topic: " + String(topic));

  // Convert payload to string
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  // Print the message for debugging
  Serial.print("Message: ");
  Serial.println(message);

  //===============================================================================
  // Handle different topics
  if (String(topic) == "esp32/led") {
    handleLED(message);
  }
  // Add more topic handlers as needed

  //===============================================================================
}

void handleLED(const char* message) {
  // Handle LED control messages
  if (strcmp(message, "ON") == 0 || strcmp(message, "1") == 0) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED turned ON");

  } else if (strcmp(message, "OFF") == 0 || strcmp(message, "0") == 0) {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED turned OFF");
  }
}

