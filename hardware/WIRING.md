Smart Baby Cradle — Wiring Notes (NodeMCU / ESP8266)

Important: NodeMCU typically has only 1 analog input labeled A0 (ADC0). Use the analog pin for a single analog device.

Component pin mapping used in this project:
- NodeMCU GND  -> all device GND pins
- NodeMCU 3V3  -> VCC for sensors & servo (servo may require 5V/regulated power if torque needed)
- NodeMCU A0   -> Sound sensor analog out (sound sensor VCC->3.3V, Gnd->GND, A0->A0)
- Wet sensor digital out -> NodeMCU D2 (digital). Use this pin for wet detection.
- Buzzer (+) -> NodeMCU D1
- Servo signal -> NodeMCU D4
- Servo VCC -> 3.3V (if servo supports 3.3V). If servo needs 5V, use an external 5V supply (common GND required).

Sample wiring steps:
1. Connect NodeMCU GND to the power rail (GND) on your breadboard.
2. Connect NodeMCU 3V3 to the VCC rail on your breadboard.
3. Plug sound sensor: Vcc->3V3, GND->GND, A0->NodeMCU A0.
4. Plug wet sensor: Vcc->3V3, GND->GND, D0->NodeMCU D2 (digital). Do NOT connect wet sensor analog to A0 if sound sensor already uses A0.
5. Connect buzzer: negative->GND, positive->D1 (use transistor if buzzer needs more current).
6. Connect servo: GND->GND, VCC->3.3V or external 5V (with common ground), signal->D4.
7. Use separate power supply for servo when possible (USB may not provide stable current under load).

Safety tip:
- Avoid powering high-current servos from the NodeMCU's 3.3V regulator. If servo jitter or resets occur, switch servo Vcc to a separate 5V regulator with common ground.
