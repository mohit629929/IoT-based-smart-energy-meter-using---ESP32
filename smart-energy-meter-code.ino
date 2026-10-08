// ============================================================
//        IoT BASED SMART ENERGY METER
//        ESP32 + ACS712 + ZMPT101B + LCD + BLYNK
// ============================================================

// ===================== BLYNK CONFIG ==========================
#define BLYNK_TEMPLATE_ID "TMPL3i3DwKOWU"
#define BLYNK_TEMPLATE_NAME "IoT Based Smart Energy Meter"
#define BLYNK_AUTH_TOKEN "3cywW1A60nYjHimFbvELXprQqwo2uGgB"

// ===================== LIBRARIES ==============================
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "ACS712.h"
#include <ZMPT101B.h>
#include <EEPROM.h>

// ===================== WIFI ===================================
char ssid[] = "Smart";
char pass[] = "12345678";

// ===================== HARDWARE ===============================
ACS712 ACS(34, 3.3, 4095, 185);
ZMPT101B voltageSensor(35, 50.0);

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===================== EEPROM =================================
#define EEPROM_SIZE 512
#define ENERGY_ADDR 0

// ===================== VARIABLES ==============================
float voltage = 0.0;
float current = 0.0;
float power = 0.0;
float energy = 0.0;
float cost = 0.0;

float rate = 7.0;

// Current calibration
float calibrationFactor = 0.035;

unsigned long previousMillis = 0;
unsigned long lastBlynkMillis = 0;
unsigned long lastEEPROMMillis = 0;

// ============================================================
// WIFI SCAN + CONNECTION
// ============================================================
bool connectToWiFi()
{
  Serial.println();
  Serial.println("================================");
  Serial.println("        WIFI SCAN");
  Serial.println("================================");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  Serial.print("Searching for WiFi: ");
  Serial.println(ssid);

  int networks = WiFi.scanNetworks();

  Serial.print("Networks found: ");
  Serial.println(networks);

  bool found = false;

  for (int i = 0; i < networks; i++)
  {
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(WiFi.SSID(i));

    Serial.print(" | Signal: ");
    Serial.print(WiFi.RSSI(i));

    Serial.println(" dBm");

    if (WiFi.SSID(i) == ssid)
    {
      found = true;
    }
  }

  Serial.println();

  if (!found)
  {
    Serial.println("ERROR: mohit HOTSPOT NOT FOUND!");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HOTSPOT NOT");
    lcd.setCursor(0, 1);
    lcd.print("FOUND!");

    return false;
  }

  Serial.println("mohit HOTSPOT FOUND!");
  Serial.println("Connecting...");

  WiFi.begin(ssid, pass);

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 40)
  {
    delay(500);

    Serial.print(".");

    attempts++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("================================");
    Serial.println("WIFI CONNECTED!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    Serial.print("Signal: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");

    Serial.println("================================");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WIFI CONNECTED");
    delay(1000);

    return true;
  }

  Serial.println("================================");
  Serial.println("WIFI CONNECTION FAILED!");
  Serial.print("Status Code: ");
  Serial.println(WiFi.status());
  Serial.println("================================");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WIFI FAILED");
  lcd.setCursor(0, 1);
  lcd.print("CHECK HOTSPOT");

  return false;
}

// ============================================================
// BLYNK CONNECTION
// ============================================================
void connectToBlynk()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Cannot connect Blynk.");
    Serial.println("WiFi is not connected.");
    return;
  }

  Serial.println();
  Serial.println("Connecting to Blynk...");

  Blynk.config(BLYNK_AUTH_TOKEN);

  if (Blynk.connect(10000))
  {
    Serial.println("================================");
    Serial.println("BLYNK CONNECTED!");
    Serial.println("SYSTEM ONLINE");
    Serial.println("================================");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BLYNK ONLINE");
    delay(1000);
  }
  else
  {
    Serial.println("BLYNK CONNECTION FAILED!");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BLYNK OFFLINE");
    delay(1000);
  }
}

// ============================================================
// SETUP
// ============================================================
void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println("       SMART ENERGY METER");
  Serial.println("================================");

  // ---------------- EEPROM ----------------
  EEPROM.begin(EEPROM_SIZE);

  energy = EEPROM.readFloat(ENERGY_ADDR);

  if (isnan(energy) || energy < 0)
  {
    energy = 0;
  }

  Serial.print("Stored Energy: ");
  Serial.print(energy, 4);
  Serial.println(" kWh");

  // ---------------- LCD ----------------
  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME");
  delay(1500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SMART ENERGY");
  lcd.setCursor(0, 1);
  lcd.print("METER");
  delay(1500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BY MOHIT");
  delay(1500);

  // ---------------- ACS712 ----------------
  Serial.println("Calibrating ACS712...");

  ACS.autoMidPoint();

  Serial.println("ACS712 Ready");

  // ---------------- ZMPT101B ----------------
  voltageSensor.setSensitivity(520.0f);

  Serial.println("ZMPT101B Ready");

  // ---------------- WIFI ----------------
  bool wifiOK = connectToWiFi();

  // ---------------- BLYNK ----------------
  if (wifiOK)
  {
    connectToBlynk();
  }

  previousMillis = millis();
  lastBlynkMillis = millis();
  lastEEPROMMillis = millis();

  lcd.clear();
}

// ============================================================
// LOOP
// ============================================================
void loop()
{
  // ==========================================================
  // WIFI RECONNECT
  // ==========================================================
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println();
    Serial.println("WiFi disconnected.");
    Serial.println("Trying to reconnect...");

    WiFi.begin(ssid, pass);

    int attempts = 0;

    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
      delay(500);
      Serial.print(".");
      attempts++;
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("WiFi Reconnected!");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());

      connectToBlynk();
    }
  }

  // ==========================================================
  // BLYNK
  // ==========================================================
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!Blynk.connected())
    {
      Serial.println("Blynk disconnected.");
      Serial.println("Trying to reconnect...");

      Blynk.connect(3000);
    }

    Blynk.run();
  }

  // ==========================================================
  // TIME
  // ==========================================================
  unsigned long now = millis();

  float dt = (now - previousMillis) / 1000.0;

  previousMillis = now;

  // ==========================================================
  // CURRENT MEASUREMENT
  // ==========================================================
  float totalCurrent = 0;

  for (int i = 0; i < 300; i++)
  {
    totalCurrent += ACS.mA_AC();
  }

  float mA = totalCurrent / 300.0;

  // Remove small noise
  if (mA < 120)
  {
    mA = 0;
  }

  current = (mA / 1000.0) * calibrationFactor;

  // ==========================================================
  // VOLTAGE MEASUREMENT
  // ==========================================================
  voltage = voltageSensor.getRmsVoltage();

  if (voltage < 20)
  {
    voltage = 0;
  }

  // ==========================================================
  // POWER
  // ==========================================================
  if (voltage == 0 || current == 0)
  {
    power = 0;
  }
  else
  {
    power = voltage * current;
  }

  // ==========================================================
  // ENERGY
  // ==========================================================
  energy += (power * dt) / 3600000.0;

  // ==========================================================
  // COST
  // ==========================================================
  cost = energy * rate;

  // ==========================================================
  // SERIAL MONITOR
  // ==========================================================
  Serial.println();
  Serial.println("--------------------------------");

  Serial.print("Voltage : ");
  Serial.print(voltage, 2);
  Serial.println(" V");

  Serial.print("Current : ");
  Serial.print(current, 3);
  Serial.println(" A");

  Serial.print("Power   : ");
  Serial.print(power, 2);
  Serial.println(" W");

  Serial.print("Energy  : ");
  Serial.print(energy, 4);
  Serial.println(" kWh");

  Serial.print("Cost    : Rs ");
  Serial.println(cost, 2);

  Serial.print("WiFi    : ");

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("CONNECTED");
  }
  else
  {
    Serial.println("DISCONNECTED");
  }

  Serial.print("Blynk   : ");

  if (Blynk.connected())
  {
    Serial.println("ONLINE");
  }
  else
  {
    Serial.println("OFFLINE");
  }

  // ==========================================================
  // LCD
  // ==========================================================
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltage, 1);

  lcd.print(" I:");
  lcd.print(current, 2);

  lcd.setCursor(0, 1);

  lcd.print("P:");
  lcd.print(power, 0);

  lcd.print(" E:");
  lcd.print(energy, 2);

  // ==========================================================
  // EEPROM
  // ==========================================================
  if (millis() - lastEEPROMMillis >= 60000)
  {
    EEPROM.writeFloat(ENERGY_ADDR, energy);
    EEPROM.commit();

    lastEEPROMMillis = millis();

    Serial.println("Energy saved to EEPROM");
  }

  // ==========================================================
  // BLYNK DATA
  // ==========================================================
  if (millis() - lastBlynkMillis >= 1000)
  {
    if (Blynk.connected())
    {
      Blynk.virtualWrite(V0, voltage);
      Blynk.virtualWrite(V1, current);
      Blynk.virtualWrite(V2, power);
      Blynk.virtualWrite(V3, energy);
      Blynk.virtualWrite(V4, cost);

      Serial.println("Blynk data sent");
    }

    lastBlynkMillis = millis();
  }

  delay(500);
}
