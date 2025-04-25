void MQTTsubscribe() {
  // Subscribe to your MQTT topics here
  client.subscribe("example");
  
  // Add additional topics as needed
  // client.subscribe("home/livingroom/temperature");
  // client.subscribe("home/garden/humidity");
}

void callBack(char* topic, byte* payload, unsigned int length) {
  // This line indicates the receipt of a message
  Serial.println("Message arrived in topic: " + String(topic));

  // Allocate a buffer to store the incoming payload
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  // Print the message for debugging
  Serial.print("Message: ");
  Serial.println(message);

  DynamicJsonDocument jsonBuffer(512);
  DeserializationError error = deserializeJson(jsonBuffer, message);

  // Reject the payload if there is an issue in deserialization
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Check if the topic is matched and if the required keys are available
  if (String(topic) == "example") {
    if (!jsonBuffer.containsKey("device") || !jsonBuffer.containsKey("State")) {
      Serial.println("Missing required keys in the message");
      return;
    }
    
    // Save each key data to variables
    const char* device = jsonBuffer["device"];
    bool state = jsonBuffer["State"];

    Serial.print("Device: ");
    Serial.println(device);
    Serial.print("State: ");
    Serial.println(state);

    if (strcmp(device, "ESP32") == 0) {
      // Do something if the device is "ESP32"
      Serial.println("This message is for this ESP32 device");
      
      if (state) {
        // Do something if state is equal to 1
        Serial.println("Turning ON something based on the message");
        // Example: digitalWrite(LED_PIN, HIGH);
      } else {
        // Do something if state is equal to 0
        Serial.println("Turning OFF something based on the message");
        // Example: digitalWrite(LED_PIN, LOW);
      }
    }
  }
  
  // Add more topic handlers as needed
  /*
  else if (String(topic) == "home/livingroom/temperature") {
    float temperature = jsonBuffer["value"];
    Serial.print("Temperature: ");
    Serial.println(temperature);
    
    // Process temperature data
  }
  */
}

void publishData() {
  // Only publish at specific intervals
  if (millis() - lastPublishTime > publishInterval) {
    lastPublishTime = millis();
    
    // Example: create a JSON document to publish
    DynamicJsonDocument doc(256);
    doc["device"] = "ESP32";
    doc["status"] = "active";
    doc["uptime"] = millis() / 1000; // uptime in seconds
    
    // Add sensor readings if available
    // doc["temperature"] = readTemperature();
    // doc["humidity"] = readHumidity();
    
    // Serialize JSON to string
    char buffer[256];
    size_t n = serializeJson(doc, buffer);
    
    // Publish to MQTT topic
    if (client.connected()) {
      if (client.publish("esp32/status", buffer, n)) {
        Serial.println("Status published successfully");
      } else {
        Serial.println("Failed to publish status");
      }
    }
  }
}