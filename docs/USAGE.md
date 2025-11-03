How the system behaves (quick summary)

- Sound detection:
  * The analog sound sensor is read continuously and smoothed.
  * When sound level stays above a calibratable threshold for >= 1 second, an alert triggers:
    - Cradle starts swinging for a set duration (5s).
    - Blynk notification sent: "Baby crying detected — cradle swinging."
    - Short buzzer beep.

- Wet detection:
  * Wet sensor digital pin triggers immediate alert when HIGH:
    - Buzzer rings continuously while wet is detected.
    - Blynk notification sent: "Cradle wet! Please check the baby."

- Cooldown:
  * Sound alerts are rate-limited via cooldown (default 30 seconds) to avoid flooding notifications.

- Servo:
  * Non-blocking swinging between 60 and 120 degrees while in swinging state.
