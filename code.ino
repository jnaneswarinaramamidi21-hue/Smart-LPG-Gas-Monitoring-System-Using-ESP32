#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Telegram credentials
#define BOT_TOKEN "YOUR_TELEGRAM_BOT_TOKEN"
#define CHAT_ID "YOUR_TELEGRAM_CHAT_ID"

// Pin configuration
#define HX711_DOUT 4
#define HX711_SCK  5

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// HX711 configuration
HX711 scale;
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// Calibration and cylinder details
float calibration_factor = -7050.0; // Calibrate for your load cell
float cylinder_tare_weight = 15.0;  // Example: empty cylinder weight in kg
float low_gas_limit = 1.0;          // Low LPG threshold in kg

// Consumption tracking
float previous_gas = 0;
float daily_consumption = 0;
unsigned long previous_time = 0;
const unsigned long DAY_INTERVAL = 86400000UL;

bool first_reading = true;
bool alert_sent = false;

void setup() {
  Serial.begin(115200);

  // Initialize LCD
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("LPG Monitoring");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  // Initialize load cell
  scale.begin(HX711_DOUT, HX711_SCK);
  scale.set_scale(calibration_factor);
  scale.tare();

  // Connect to WiFi
  WiFi.begin(ssid, password);

  lcd.clear();
  lcd.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Telegram HTTPS connection
  client.setInsecure();

  // Initialize time using NTP
  configTime(19800, 0, "pool.ntp.org");

  bot.sendMessage(CHAT_ID,
                  "Smart LPG Monitoring System Started!",
                  "");

  lcd.clear();
}

void loop() {

  // Read cylinder weight
  float total_weight = scale.get_units(10);

  // Calculate remaining LPG
  float remaining_gas =
      total_weight - cylinder_tare_weight;

  if (remaining_gas < 0) {
    remaining_gas = 0;
  }

  // First measurement
  if (first_reading) {
    previous_gas = remaining_gas;
    previous_time = millis();
    first_reading = false;
  }

  // Calculate daily consumption
  if (millis() - previous_time >= DAY_INTERVAL) {

    daily_consumption = previous_gas - remaining_gas;

    previous_gas = remaining_gas;
    previous_time = millis();

    Serial.print("Daily consumption: ");
    Serial.println(daily_consumption);
  }

  // Predict remaining days
  float remaining_days = 0;

  if (daily_consumption > 0.01) {
    remaining_days =
        remaining_gas / daily_consumption;
  }

  // LCD display
  lcd.setCursor(0, 0);
  lcd.print("LPG: ");
  lcd.print(remaining_gas, 2);
  lcd.print(" kg   ");

  lcd.setCursor(0, 1);

  if (daily_consumption > 0.01) {
    lcd.print("Days left: ");
    lcd.print((int)remaining_days);
    lcd.print("   ");
  } else {
    lcd.print("Calculating use ");
  }

  // Serial monitor
  Serial.print("Total Weight: ");
  Serial.println(total_weight);

  Serial.print("Remaining LPG: ");
  Serial.println(remaining_gas);

  Serial.print("Daily Consumption: ");
  Serial.println(daily_consumption);

  Serial.print("Estimated Days: ");
  Serial.println(remaining_days);

  // Telegram low gas alert
  if (remaining_gas <= low_gas_limit &&
      !alert_sent) {

    String message =
      "LOW LPG ALERT!\n";

    message += "Remaining LPG: ";
    message += String(remaining_gas, 2);
    message += " kg\n";

    message += "Please arrange for cylinder refill.";

    bot.sendMessage(CHAT_ID, message, "");

    alert_sent = true;
  }

  // Reset alert after refill
  if (remaining_gas > low_gas_limit + 0.5) {
    alert_sent = false;
  }

  delay(3000);
}
