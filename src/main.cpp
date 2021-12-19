#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <secrets.h>

WiFiClient espClient;
PubSubClient client(espClient);

EspClass ESPm;

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqtt_server = "192.168.1.xyz"; // The IP-Adress of your MQTT-Server
const unsigned int mqtt_port = 1883;
const char* mqtt_user = "admin";
const char* mqtt_pass = SECRET_MQTT_PASS;

IPAddress ip(192, 168, 1, FIXED_IP); // If your router reserves a fixed IP for your ESP, enter it here. Alternatively, you can also use the placeholder of the "secrets.h" file.
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

ADC_MODE(ADC_VCC);
int voltage = 999;

String clientId = "iot-Bathroom";

bool status; // Windowsstatus - 0 = open / 1 = closed
int counter = 0;

void setup_wifi(){  // Start WiFi-Connection
  WiFi.config(ip, gateway, subnet);  // Disable this line for the automatic assignment of the IP address by the DHCP of your router
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    counter++;
    if (counter > 20) {
      ESP.deepSleep(0);
    }
  }
  counter = 0;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
    } else {
      counter++;
      delay(500);
    }
    if (counter > 20) {
      ESP.deepSleep(0);
    }
  }
  counter = 0;
}

void setup() {
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  pinMode(13,INPUT);
  pinMode(D0, WAKEUP_PULLUP);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  voltage = ESP.getVcc();
  status = digitalRead(13);
  if (status == 0) {
    client.publish("Windows/Bathroom", "open", true);
    client.publish("Windows/Bathroom/Vcc", String(voltage).c_str());
  } else {
    client.publish("Windows/Bathroom", "closed", true);
    client.publish("Windows/Bathroom/Vcc", String(voltage).c_str());
  }
  client.subscribe("Windows/Bathroom");
  delay(200);
  
  ESP.deepSleep(0);
  delay(200);
}