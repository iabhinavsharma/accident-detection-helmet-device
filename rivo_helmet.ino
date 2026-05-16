#include <Wire.h>
#include <TinyGPS++.h>
#include "BluetoothSerial.h"
#include <math.h>

BluetoothSerial SerialBT;
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

// ================= MPU6050 =================
#define MPU_ADDR 0x68

int16_t AcX, AcY, AcZ;
int16_t GyX, GyY, GyZ;

// ================= Pins =================
#define BUZZER_PIN 27
#define BUTTON_PIN 26
#define BATTERY_PIN 34

// GPS
// GPS TX -> GPIO16
// GPS RX -> GPIO17

// ================= Battery =================
const float CALIBRATION_FACTOR = 1.089;

// ================= Improved Thresholds =================
// More realistic values to reduce false alerts

const int IMPACT_THRESHOLD = 30000;
const int GYRO_THRESHOLD = 20000;
const int ROLL_THRESHOLD = 65;

// ================= States =================
bool alertActive = false;

unsigned long alertStartTime = 0;

int lastSecondShown = -1;
unsigned long lastBatteryUpdate = 0;

// Bluetooth state
bool lastBTState = false;


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  // Bluetooth
  SerialBT.begin("RIVO");

  // I2C
  Wire.begin();

  // Wake MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // GPS
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);

  // Battery ADC
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  // Buzzer Safe Init
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  noTone(BUZZER_PIN);

  // Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.println("SYSTEM READY");
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  // =================================================
  // BLUETOOTH CONNECT / DISCONNECT SOUND
  // =================================================

  bool currentBTState = SerialBT.hasClient();

  // CONNECTED SOUND
  if (currentBTState && !lastBTState) {

    Serial.println("Bluetooth Connected");

    tone(BUZZER_PIN, 1500);
    delay(150);

    tone(BUZZER_PIN, 2200);
    delay(150);

    noTone(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // DISCONNECTED SOUND
  if (!currentBTState && lastBTState) {

    Serial.println("Bluetooth Disconnected");

    tone(BUZZER_PIN, 2200);
    delay(200);

    tone(BUZZER_PIN, 1200);
    delay(300);

    noTone(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, LOW);
  }

  lastBTState = currentBTState;

  // =================================================
  // GPS READ
  // =================================================

  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  // =================================================
  // MPU6050 READ
  // =================================================

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_ADDR, 14, true);

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  Wire.read();
  Wire.read();

  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  // =================================================
  // ROLL
  // =================================================

  float roll = atan2(AcY, AcZ) * 180 / PI;

  // =================================================
  // ACCIDENT DETECTION
  // =================================================

  bool impactDetected = false;

  if (
      abs(AcX) > IMPACT_THRESHOLD ||
      abs(AcY) > IMPACT_THRESHOLD ||
      abs(AcZ) > IMPACT_THRESHOLD ||
      abs(GyX) > GYRO_THRESHOLD ||
      abs(GyY) > GYRO_THRESHOLD ||
      abs(GyZ) > GYRO_THRESHOLD ||
      abs(roll) > ROLL_THRESHOLD
     )
  {
    impactDetected = true;
  }

  // =================================================
  // BATTERY
  // =================================================

  int raw = analogRead(BATTERY_PIN);

  float adcVoltage = raw * (3.3 / 4095.0);

  float batteryVoltage = adcVoltage * 2.0;

  batteryVoltage *= CALIBRATION_FACTOR;

  int batteryPercent =
      map(batteryVoltage * 100, 300, 420, 0, 100);

  batteryPercent = constrain(batteryPercent, 0, 100);

  // =================================================
  // START ALERT
  // =================================================

  if (impactDetected && !alertActive) {

    alertActive = true;

    alertStartTime = millis();

    lastSecondShown = -1;

    Serial.println("ACCIDENT DETECTED!");

    SerialBT.println("================================");
    SerialBT.println("ALERT: ACCIDENT DETECTED!");
    SerialBT.println("Press button or type CANCEL");
    SerialBT.println("================================");

    // BATTERY INFO
    SerialBT.print("Battery: ");
    SerialBT.print(batteryVoltage);
    SerialBT.print("V  ");
    SerialBT.print(batteryPercent);
    SerialBT.println("%");

    // MPU INFO
    SerialBT.print("Roll: ");
    SerialBT.println(roll);

    SerialBT.print("Accel X: ");
    SerialBT.println(AcX);

    SerialBT.print("Accel Y: ");
    SerialBT.println(AcY);

    SerialBT.print("Accel Z: ");
    SerialBT.println(AcZ);

    // GPS INFO
    if (gps.location.isValid()) {

      float latitude = gps.location.lat();
      float longitude = gps.location.lng();

      SerialBT.print("Latitude: ");
      SerialBT.println(latitude, 6);

      SerialBT.print("Longitude: ");
      SerialBT.println(longitude, 6);

      SerialBT.print("Maps: https://maps.google.com/?q=");
      SerialBT.print(latitude, 6);
      SerialBT.print(",");
      SerialBT.println(longitude, 6);
    }
    else {

      SerialBT.println("GPS NOT FIXED");
    }

    SerialBT.println();
  }

  // =================================================
  // ALERT HANDLING
  // =================================================

  if (alertActive) {

    // Emergency buzzer
    tone(BUZZER_PIN, 2000);

    // COUNTDOWN
    int secondsLeft =
      10 - ((millis() - alertStartTime) / 1000);

    if (secondsLeft != lastSecondShown &&
        secondsLeft >= 0) {

      lastSecondShown = secondsLeft;

      Serial.print("Sending alert in: ");
      Serial.println(secondsLeft);

      SerialBT.print("Sending alert in: ");
      SerialBT.print(secondsLeft);
      SerialBT.println(" sec");
    }

    // BUTTON CANCEL
    if (digitalRead(BUTTON_PIN) == LOW) {

      noTone(BUZZER_PIN);
      digitalWrite(BUZZER_PIN, LOW);

      alertActive = false;

      Serial.println("ALERT CANCELLED");

      SerialBT.println("ALERT CANCELLED BY BUTTON");
      SerialBT.println();

      delay(500);
    }

    // APP CANCEL
    if (SerialBT.available()) {

      String command = SerialBT.readStringUntil('\n');

      command.trim();

      if (command == "CANCEL") {

        noTone(BUZZER_PIN);
        digitalWrite(BUZZER_PIN, LOW);

        alertActive = false;

        Serial.println("ALERT CANCELLED FROM APP");

        SerialBT.println("ALERT CANCELLED FROM APP");
        SerialBT.println();

        delay(500);
      }
    }

    // ALERT SENT
    if (millis() - alertStartTime >= 10000) {

      noTone(BUZZER_PIN);
      digitalWrite(BUZZER_PIN, LOW);

      alertActive = false;

      Serial.println("ALERT SENT");

      SerialBT.println("ALERT SENT");
      SerialBT.println();
    }
  }

  // =================================================
// SEND BATTERY EVERY 5 SECONDS
// =================================================

if (millis() - lastBatteryUpdate >= 5000) {

  lastBatteryUpdate = millis();

  SerialBT.println("===== BATTERY =====");

  SerialBT.print("Battery Voltage: ");
  SerialBT.print(batteryVoltage);
  SerialBT.println(" V");

  SerialBT.print("Battery Percentage: ");
  SerialBT.print(batteryPercent);
  SerialBT.println("%");

  SerialBT.println();
}

  delay(100);
}