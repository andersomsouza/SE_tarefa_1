/*
    Firmware para ESP8266 desenvolvido com o core do Arduino
    Utiliza a plataforma oficial do esp8266/arduino
    Utiliza a bibilioteca MQTTClient https://github.com/256dpi/arduino-mqtt

*/

#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include <Servo.h>

#define ESP_MQTT_ID 0

const char ssid[] = "AnderHome_2.4";
const char pass[] = "nescafe12345";
Servo servoVertical;
Servo servoHorizontal;

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect();  // <- predefine connect() for setup()

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  servoVertical.attach(D5);
  servoHorizontal.attach(D6);

  pinMode(LED_BUILTIN, OUTPUT);
  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
  // You need to set the IP address directly.
  //
  // MQTT brokers usually use port 8883 for secure connections.
  client.begin("192.168.0.2", net);
  client.onMessage(messageReceived);

  connect();
}

String topicoBase = String("/esp8266/") + String(ESP_MQTT_ID);
void connect() {
  Serial.print("Conectando Wi-FI...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nConectandoBroker...");
  String clientName = String("esp8266_ander/") + ESP_MQTT_ID;
  char clientNameChar[20];
  clientName.toCharArray(clientNameChar, 20);
  while (!client.connect(clientNameChar, "ander", "ander123")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nConectado!");
  client.subscribe(topicoBase + "/led");
  client.subscribe(topicoBase + "/servo/y");
  client.subscribe(topicoBase + "/servo/x");
}

void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish(topicoBase + "/alive", String(millis() / 1000));
  }

}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if (topic.equals(topicoBase + "/led")) {
    Serial.println(String("É led ") + payload.toInt());
    digitalWrite(LED_BUILTIN, !payload.toInt());
  } else if (topic.equals(topicoBase + "/servo/y")) {
    servoVertical.write(payload.toInt());
  } else if (topic.equals(topicoBase + "/servo/x")) {
    servoHorizontal.write(payload.toInt());
  }
}
