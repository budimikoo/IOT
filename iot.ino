#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- WiFi ---r
const char* ssid = "Riyanti House";
const char* password = "segojagung";

// --- MQTT ---
const char* mqtt_server = "mqtt.revolusi-it.com";
const int mqtt_port = 1883;
const char* mqtt_user = "usm";
const char* mqtt_pass = "usmjaya1";
const char* topik = "iot/G.231.22.0128"; // Ganti sesuai NIM kamu

// --- PIN ---
#define LED1_PIN D5
#define LED2_PIN D6
#define LED3_PIN D8
#define DHTPIN D4
#define DHTTYPE DHT11
#define LM35_PIN A0

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat I2C

void setup_wifi() {
  delay(10);
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 30) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Terhubung");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nGagal konek WiFi.");
  }
}

void setLED(int level) {
  digitalWrite(LED1_PIN, level >= 1);
  digitalWrite(LED2_PIN, level >= 2);
  digitalWrite(LED3_PIN, level >= 3);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan MQTT... ");
    String clientId = "G.231.22.0137"; // Sesuaikan dengan NIM
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("Terhubung ke MQTT");
      client.subscribe(topik);
    } else {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi 5 detik...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("MQTT IoT Ready");

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float suhuDHT = dht.readTemperature();
  float kelembaban = dht.readHumidity();

  if (isnan(suhuDHT) || isnan(kelembaban)) {
    Serial.println("Gagal baca DHT11!");
    return;
  }

  int nilaiAnalog = analogRead(LM35_PIN);
  float volt = nilaiAnalog * (3.3 / 1023.0);
  float suhuLM35 = volt * 100.0;

  int ledLevel;
  if (suhuDHT > 30 || suhuLM35 > 30) {
    ledLevel = 3;
  } else if (suhuDHT >= 25 || suhuLM35 >= 25) {
    ledLevel = 2;
  } else {
    ledLevel = 1;
  }

  setLED(ledLevel);

  String payload = "{";
  payload += "\"suhuDHT\":" + String(suhuDHT, 1) + ",";
  payload += "\"kelembaban\":" + String(kelembaban, 1) + ",";
  payload += "\"suhuLM35\":" + String(suhuLM35, 1) + ",";
  payload += "\"LED\":" + String(ledLevel);
  payload += "}";

  Serial.println("Kirim MQTT: " + payload);
  client.publish(topik, payload.c_str());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DHT:");
  lcd.print(suhuDHT, 1);
  lcd.print((char)223);
  lcd.print("C H:");
  lcd.print(kelembaban, 0);

  lcd.setCursor(0, 1);
  lcd.print("LM35:");
  lcd.print(suhuLM35, 1);
  lcd.print((char)223);
  lcd.print("C L:");
  lcd.print(ledLevel);

  delay(5000);
}