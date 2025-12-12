// ESP32 Stress Monitor + LED Blinking + POST ke https://devtunnels.ms (FINAL & SUDAH JALAN)
// Auth pakai API Key device (espstresslifetracker) — TIDAK PERLU LOGIN / JWT

#define BLYNK_TEMPLATE_ID   "TMPL6QllfqjzJ"
#define BLYNK_TEMPLATE_NAME "Stress Detection Monitor"
#define BLYNK_AUTH_TOKEN    "FAReJFS0_ZTPkdBec-iCUkfKDe_NQmWd"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "SensorManager.h"
#include "DisplayManager.h"

// KONFIGURASI
#define BUZZER_PIN     25
#define LED_GREEN_PIN  26
#define LED_RED_PIN    27

// URL
const char* API_HOST = "https://6865l8g3-8000.asse.devtunnels.ms/api/predict/";
const char* API_KEY  = "espstresslifetracker";

// WiFi
char ssid[] = "Warung Solo";
char pass[] = "budesolo10";

// Objek class
SensorManager sensorMgr;
DisplayManager displayMgr;

// Status
String serverStatusLabel = "Membaca...";  // low, medium, high
int serverCluster = -1;                   // 0, 1, 2

// Bunyi Buzzer
void playAlarmSound() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
}

void playSuccessSound() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(600);
  digitalWrite(BUZZER_PIN, LOW);
}

// Kirim data ke server
void sendDataToServer(int bpm, int spo2, float rmssd, float sdrr, int pnn50) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi tidak terhubung!");
    return;
  }

  HTTPClient http;
  http.setTimeout(20000);
  http.begin(API_HOST);

  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-Api-Key", API_KEY);

  String payload = "{";
  payload += "\"api_key\":\"" + String(API_KEY) + "\",";
  payload += "\"heart_rate\":" + String(bpm) + ",";
  payload += "\"spo2\":" + String(spo2) + ",";
  payload += "\"rmssd\":" + String(rmssd, 2) + ",";
  payload += "\"sdrr\":" + String(sdrr, 2) + ",";
  payload += "\"pnn50\":" + String(pnn50);
  payload += "}";

  Serial.println("\nMENGIRIM KE SERVER");
  Serial.println("URL : " + String(API_HOST));
  Serial.println("Payload : " + payload);

  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpCode);
    String response = http.getString();
    Serial.println("Response: " + response);

    // Ambil response
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      serverStatusLabel = "ERROR";
      serverCluster = -1;
      return;
    }

    serverCluster = doc["predicted_cluster"] | -1;
    const char* label = doc["label"];
    serverStatusLabel = label ? String(label) : "UNKNOWN";

    Serial.print("Cluster: ");
    Serial.println(serverCluster);
    Serial.print("Label: ");
    Serial.println(serverStatusLabel);

  } else {
    Serial.print("HTTP Error code: ");
    Serial.println(httpCode);
    Serial.println("Error detail: " + String(http.errorToString(httpCode)));
    serverStatusLabel = "TIMEOUT";
    serverCluster = -1;
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  digitalWrite(LED_GREEN_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  displayMgr.init();
  sensorMgr.init();

  displayMgr.showMessage("Menghubungkan WiFi...");

  // WiFi + Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  displayMgr.showMessage("Terhubung!\nSiap deteksi");
  delay(2000);
}

void loop() {
  Blynk.run();

  // Update sensor
  sensorMgr.updateSensor();
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 1000) {
    lastUpdate = millis();

    sensorMgr.calculateHRV();
    sensorMgr.checkStability();

    // Freeze logic
    if (sensorMgr.isMeasurementDone() && !sensorMgr.isFrozen()) {
      sensorMgr.freezeValues();
      sendDataToServer(sensorMgr.getBPM(), sensorMgr.getSpO2(), sensorMgr.getRMSSD(), sensorMgr.getSDRR(), sensorMgr.getPNN50());

      bool abnormal = (serverStatusLabel == "high" || serverStatusLabel == "medium");

      if (abnormal) playAlarmSound(); else playSuccessSound();
      sensorMgr.setFinalStatus(serverStatusLabel);
    }

    // LED indicator
    static bool ledState = false;
    ledState = !ledState;
    bool abnormal = (serverStatusLabel == "high" || serverStatusLabel == "medium");

    digitalWrite(LED_RED_PIN, sensorMgr.isFrozen() && abnormal && ledState ? HIGH : LOW);
    digitalWrite(LED_GREEN_PIN, sensorMgr.isFrozen() && !abnormal && ledState ? HIGH : LOW);

    // Update Blynk
    int bpm = sensorMgr.getBPM();
    if (bpm > 30) {
      Blynk.virtualWrite(V0, bpm);
      Blynk.virtualWrite(V1, sensorMgr.getSpO2());
      Blynk.virtualWrite(V2, sensorMgr.getRMSSD());
      Blynk.virtualWrite(V3, sensorMgr.getSDRR());
      Blynk.virtualWrite(V4, sensorMgr.getPNN50());

      if (sensorMgr.isFrozen() && !sensorMgr.isDoneNotified()) {
        String statusUpper = serverStatusLabel;
        statusUpper.toUpperCase();
        Blynk.logEvent(abnormal ? "stress_alert" : "done_alert", statusUpper);
        sensorMgr.setDoneNotified(true);
      }

      Blynk.virtualWrite(V5, sensorMgr.getStatus());
    }
  }

  // Update display (100ms)
  displayMgr.updateDisplay(
    sensorMgr.getBPM(),
    sensorMgr.getSpO2(),
    sensorMgr.getRMSSD(),
    sensorMgr.getSDRR(),
    sensorMgr.getPNN50(),
    sensorMgr.getStatus(),
    sensorMgr.isFingerDetected(),
    sensorMgr.isFrozen()
  );
}