/*
  SmartCradle.ino
  Smart Baby Cradle Monitoring System (ESP8266 NodeMCU)
  - Uses: Sound sensor (A0), Wet sensor (D2 digital), Buzzer (D1), Servo (D4)
  - Blynk (legacy) used for notifications and telemetry
  - wifi_config.h is NOT included in repo; copy wifi_config.h.example to wifi_config.h and fill it.

  Pins (as used in this project):
   - SOUND_A_PIN    -> A0 (analog)
   - WET_DIGITAL_PIN -> D2
   - BUZZER_PIN -> D1
   - SERVO_PIN -> D4

  Created: 2025
  Author: {author_name}
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include "wifi_config.h"   // Local file: create from wifi_config.h.example (do NOT commit real wifi_config.h)

//
// === User-configurable thresholds & timings ===
//
const int SOUND_SAMPLES = 50;
int soundThreshold = 400;      // analog threshold for cry detection (0-1023). Calibrate with serial.
unsigned long soundDetectMillis = 0;
const unsigned long SOUND_MIN_ACTIVE_MS = 1000; // require sound above threshold for this ms before triggering

const unsigned long ALERT_COOLDOWN_MS = 30UL * 1000UL; // 30 seconds between same alerts
unsigned long lastSoundAlert = 0;
unsigned long lastWetAlert = 0;

// Pins (NodeMCU pin names)
const int SOUND_A_PIN = A0;   // Analog pin for sound sensor
const int WET_DIGITAL_PIN = D2; // Digital output of wet sensor (D0 from sensor mapped to NodeMCU D2)
const int BUZZER_PIN = D1;    // Buzzer (+)
const int SERVO_PIN = D4;     // Servo signal

// Blynk virtual pins
const int V_SOUND_ALERT = V1;
const int V_WET_ALERT = V2;
const int V_SOUND_LEVEL = V3;
const int V_WET_LEVEL = V4;
const int V_SERVO_STATE = V5; // show servo swinging state

// Servo control
Servo cradleServo;
int servoAngle = 90;         // neutral position
bool swinging = false;
unsigned long swingStart = 0;
const unsigned long SWING_DURATION_MS = 5UL * 1000UL; // swing for 5 seconds
unsigned long lastSwingMove = 0;
const unsigned long SWING_MOVE_INTERVAL = 300; // ms between servo position updates
int swingDir = 1;

// Wi-Fi reconnection params handled internally by Blynk

BlynkTimer timer;

// --- Exponential moving average for smoothing sound ---
float soundEMA = 0.0;
const float EMA_ALPHA = 0.08;

void checkSensors();
void triggerSoundAlert();
void triggerWetAlert();
void startSwinging();
void stopSwinging();
void updateServo();

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println();
  Serial.println("Smart Baby Cradle - Starting...");

  // Pins
  pinMode(WET_DIGITAL_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Servo
  cradleServo.attach(SERVO_PIN);
  cradleServo.write(servoAngle); // start at neutral

  // Blynk init
  Blynk.begin(BLYNK_AUTH, WIFI_SSID, WIFI_PASS);

  // Timer: poll sensors at 200ms
  timer.setInterval(200L, checkSensors);

  // Periodic telemetry to blynk every 2s
  timer.setInterval(2000L, []() {
    float slevel = analogRead(SOUND_A_PIN); // 0-1023
    Blynk.virtualWrite(V_SOUND_LEVEL, (int)slevel);
    int wet = digitalRead(WET_DIGITAL_PIN);
    Blynk.virtualWrite(V_WET_LEVEL, wet);
    Blynk.virtualWrite(V_SERVO_STATE, swinging ? 1 : 0);
  });

  Serial.println("Setup complete.");
}

void loop() {
  Blynk.run();
  timer.run();
  updateServo();
}

void checkSensors() {
  // Read analog sound sensor and smooth it
  int rawSound = analogRead(SOUND_A_PIN);
  soundEMA = (EMA_ALPHA * rawSound) + (1 - EMA_ALPHA) * soundEMA;

  unsigned long now = millis();
  if (soundEMA > soundThreshold) {
    if (soundDetectMillis == 0) soundDetectMillis = now;
    else if (now - soundDetectMillis >= SOUND_MIN_ACTIVE_MS) {
      if (now - lastSoundAlert > ALERT_COOLDOWN_MS) {
        triggerSoundAlert();
        lastSoundAlert = now;
      }
      soundDetectMillis = 0; // reset detection
    }
  } else {
    soundDetectMillis = 0;
  }

  // Wet sensor digital read (D2)
  int wetDigital = digitalRead(WET_DIGITAL_PIN);
  if (wetDigital == HIGH) { // sensor detects water
    // turn buzzer ON while wet
    digitalWrite(BUZZER_PIN, HIGH);
    if (now - lastWetAlert > ALERT_COOLDOWN_MS) {
      triggerWetAlert(); // sends blynk notify and telemetry
      lastWetAlert = now;
    }
  } else {
    // turn buzzer OFF when no wetness
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void triggerSoundAlert() {
  Serial.println("Sound alert: Baby cry detected!");
  // start cradle swinging
  startSwinging();

  // Notify via Blynk
  Blynk.notify("Baby crying detected — cradle swinging.");

  // telemetry
  Blynk.virtualWrite(V_SOUND_ALERT, 1);

  // Short feedback beep
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
}

void triggerWetAlert() {
  Serial.println("Wet alert: Moisture detected!");

  // Notify via Blynk
  Blynk.notify("Cradle wet! Please check the baby.");

  // telemetry
  Blynk.virtualWrite(V_WET_ALERT, 1);
}

// Start swinging routine (non-blocking)
void startSwinging() {
  swinging = true;
  swingStart = millis();
  lastSwingMove = 0;
  swingDir = 1;
  Serial.println("Starting swinging sequence.");
}

// Stop swinging
void stopSwinging() {
  swinging = false;
  servoAngle = 90;
  cradleServo.write(servoAngle);
  Serial.println("Stopped swinging sequence.");
}

void updateServo() {
  if (!swinging) return;

  unsigned long now = millis();
  if (now - swingStart > SWING_DURATION_MS) {
    stopSwinging();
    return;
  }

  if (now - lastSwingMove < SWING_MOVE_INTERVAL) return;

  // oscillate servo between 60 and 120 degrees
  const int MIN_ANGLE = 60;
  const int MAX_ANGLE = 120;
  const int STEP = 8;

  servoAngle += swingDir * STEP;
  if (servoAngle >= MAX_ANGLE) {
    servoAngle = MAX_ANGLE;
    swingDir = -1;
  } else if (servoAngle <= MIN_ANGLE) {
    servoAngle = MIN_ANGLE;
    swingDir = 1;
  }
  cradleServo.write(servoAngle);
  lastSwingMove = now;
}
