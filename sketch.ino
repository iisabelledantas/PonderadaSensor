#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define DHTPIN 15
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* backendUrl = "http://hyperrational-radiantly-jamari.ngrok-free.dev/api/telemetria/dados";

void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.print("Conectando ao Wi-Fi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(5000);

  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("Falha ao ler o sensor DHT22!");
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(backendUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("ngrok-skip-browser-warning", "true");

    String payload = "{";
    payload += "\"sensorId\":\"sensor-001\",";
    payload += "\"temperatura\":" + String(temperatura, 1) + ",";
    payload += "\"umidade\":" + String(umidade, 1) + ",";
    payload += "\"timestamp\":\"2024-01-01T00:00:00\"";
    payload += "}";

    Serial.println("Enviando: " + payload);

    int httpCode = http.POST(payload);

    Serial.print("Resposta HTTP: ");
    Serial.println(httpCode);

    http.end();
  } else {
    Serial.println("Wi-Fi desconectado!");
  }
}