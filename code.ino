#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

#define PIR_PIN 14
#define RELAY_PIN 13
#define DHT_PIN 12
#define DHTTYPE DHT22

DHT dht(DHT_PIN, DHTTYPE);

const char* ssid = "Smart_IoT_Node"; //server
const char* password = "12345678"; //password

WebServer server(80);

bool manualControl = false;
bool relayState = false;

void handleRoot() {

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  bool motion = digitalRead(PIR_PIN);

  String page = "<!DOCTYPE html><html><head><title>IoT Control</title>";
  page += "<meta http-equiv='refresh' content='2'>";
  page += "<style>body{text-align:center;font-family:Arial;background:#f2f2f2;}";
  page += "button{padding:15px 25px;font-size:18px;margin:10px;border-radius:10px;border:none;}";
  page += ".on{background:green;color:white;} .off{background:red;color:white;}";
  page += "</style></head><body>";

  page += "<h1>ESP32 Smart Node</h1>";

  page += "<p><b>Temperature:</b> " + String(temperature) + " °C</p>";
  page += "<p><b>Humidity:</b> " + String(humidity) + " %</p>";
  page += "<p><b>PIR Motion:</b> " + String(motion ? "Detected" : "No Motion") + "</p>";
  page += "<p><b>Relay State:</b> " + String(relayState ? "ON" : "OFF") + "</p>";

  page += "<button class='on' onclick=\"location.href='/relayON'\">Relay ON</button>";
  page += "<button class='off' onclick=\"location.href='/relayOFF'\">Relay OFF</button>";
  page += "<br><button onclick=\"location.href='/auto'\">Auto Mode</button>";

  page += "</body></html>";

  server.send(200, "text/html", page);
}

void relayON() {
  manualControl = true;
  relayState = true;
  digitalWrite(RELAY_PIN, HIGH);
  server.sendHeader("Location", "/");
  server.send(303);
}

void relayOFF() {
  manualControl = true;
  relayState = false;
  digitalWrite(RELAY_PIN, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void autoMode() {
  manualControl = false;
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  dht.begin();

  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/relayON", relayON);
  server.on("/relayOFF", relayOFF);
  server.on("/auto", autoMode);

  server.begin();
}

void loop() {
  server.handleClient();

  if (!manualControl) {
    bool motion = digitalRead(PIR_PIN);

    if (motion) {
      relayState = true;
      digitalWrite(RELAY_PIN, HIGH);
    } else {
      relayState = false;
      digitalWrite(RELAY_PIN, LOW);
    }
  }
}