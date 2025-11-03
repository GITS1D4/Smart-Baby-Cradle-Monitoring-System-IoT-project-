# Smart Baby Cradle Monitoring System (ESP8266 NodeMCU)

A simple IoT-enabled smart baby cradle using ESP8266 NodeMCU.  
Monitors baby sounds and cradle wetness and notifies caregivers via **Blynk** while performing local actions (swing cradle using servo, ring buzzer).

---

## Features
- Detect baby crying via analog sound sensor -> start cradle swinging + Blynk notification
- Detect moisture/wetness via wet sensor -> buzzer + Blynk notification
- Non-blocking servo control (swing for configurable duration)
- Cooldown to avoid repeated alerts
- Simple telemetry: sound level, wet status, servo state sent to Blynk

---

## Author
Developed by **Rohini Sharanya P**

---

## Files included
- `SmartCradle.ino` — main sketch (ESP8266 NodeMCU)
- `wifi_config.h.example` — example Wi-Fi / Blynk config file (rename locally to `wifi_config.h`)
- `hardware/WIRING.md` — wiring instructions and notes
- `hardware/BOM.md` — bill of materials
- `docs/USAGE.md` — how the app behaves
- `docs/blynk_config_example.json` — example Blynk dashboard config
- `.gitignore`, `LICENSE`

---

## Hardware (what I used)
- ESP8266 NodeMCU (v1.0)
- Analog sound detection module (microphone module)
- Wet/Rain detection sensor (digital output used)
- Micro servo (SG90 or equivalent)
- Active buzzer
- Breadboard, jumper wires, USB cable

See `hardware/BOM.md` for details.

---

## Wiring (summary)
See `hardware/WIRING.md`. Key pins used in code:
- SOUND (analog) -> NodeMCU A0
- WET (digital)   -> D2
- BUZZER          -> D1
- SERVO_SIGNAL    -> D4
- All GNDs tied together
- Use 3.3V for sensors. If servo requires 5V, use an external regulated 5V supply with common ground.

---

## Software & Libraries (Arduino IDE)
Install the following before uploading:
1. Arduino IDE (latest)
2. In Arduino > Preferences > Additional Boards Manager URLs:
   - `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
3. Boards Manager -> install **esp8266** by ESP8266 Community
4. Library Manager -> install:
   - **Blynk** (Blynk library)
   - **Servo** (if not present; Arduino built-in or install via Library Manager)
   - (Optional) **ESP8266WiFi** is included with the board package

---

## Setup (do this **before** uploading)
1. Copy `wifi_config.h.example` -> `wifi_config.h` (in the same folder as the sketch).
   - Fill in `WIFI_SSID`, `WIFI_PASS`, `BLYNK_AUTH` values.
2. Make sure you **do not commit** `wifi_config.h` to the repo (it is listed in `.gitignore`).

---

## How to upload (step-by-step, Arduino IDE)

1. Open Arduino IDE.
2. Install ESP8266 board support (see "Software & Libraries" above).
3. Open `SmartCradle.ino` in Arduino IDE.
4. Select board: `NodeMCU 1.0 (ESP-12E Module)` (or matching NodeMCU board).
5. Select correct Port (Tools > Port).
6. Ensure `wifi_config.h` (not example) exists in the same folder as the sketch.
7. Click Upload. Monitor output; when complete, open Serial Monitor at `115200` to see debug logs.

---

## Blynk setup (legacy app)
1. Install the legacy Blynk app on your phone.
2. Create a new project; choose device **ESP8266** and get the Auth Token (single token).
3. Copy the Auth Token to `wifi_config.h` (`BLYNK_AUTH`).
4. Add Notification widget in Blynk project to receive push notifications.
5. (Optional) Add Value widgets to display V3 (sound level) and V4 (wet flag).

---

## Calibration
- The `soundThreshold` value in `SmartCradle.ino` is set to `400` by default — tune this for your environment.
- Open Serial Monitor and read the analog sound value while you clap or simulate crying. Increase/decrease `soundThreshold` so that normal ambient noise is below threshold and crying is above.

---

## Safety, power & reliability notes
- Many servos require 5V and can draw >400mA. Using NodeMCU 3.3V pin to power a servo can cause instability. If your servo behaves jittery or NodeMCU resets, use an external 5V supply (common ground).
- Do not rely on this prototype as a medical-grade monitoring device. It's a learning/demo project.
- Keep network credentials secure — do not publish `wifi_config.h`.

---

## Next steps & improvements (roadmap)
- Add TinyML: collect labeled audio to classify cry vs non-cry and run a small TFLite model on-device.
- Replace Blynk with MQTT+TLS or HTTPS for better security.
- Add a rolling local log to survive connectivity loss.
- Create an onboarding flow (secure provisioning) to avoid embedding Blynk tokens.

---

## Troubleshooting
- If servo jitter / resets: use separate power supply for servo and ensure common ground.
- If Blynk disconnects: check WiFi credentials, router, and ensure your phone can receive Blynk notifications.
- If both sound and wet sensors need analog pins: NodeMCU has a single A0 — use digital output for wet sensor or add an external ADC (e.g., ADS1115).

---

## Contributing
If you improve the project, open a PR — include wiring photos and test logs.

---

## License
MIT (see LICENSE file)
