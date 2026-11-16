/*
 * String light controller (Uno R3)
 *
 * Replaces the 3xAAA pack. Lights can be controlled by:
 *   1. IR remote from the RexQualis kit (OK = on/off, RIGHT = next mode,
 *      UP/DOWN = brighter/dimmer)
 *   2. A local push button (short press = on/off, long press = next mode)
 *   3. Serial commands over USB at 9600 baud, newline-terminated:
 *        on | off | toggle | mode | dim <0-255> | status
 *      This is how a PC, Pi or Home Assistant drives it later.
 *
 * Wiring (parts are all from the two kits):
 *
 *   POWER (low-side switch, PN2222):
 *     Uno 5V --> 1N4007 anode, cathode --> battery box (+)   (diode drops ~0.7V, so ~4.3V like fresh AAAs)
 *     battery box (-) --> PN2222 collector
 *     PN2222 emitter --> Uno GND
 *     Uno D9 --> 1k resistor --> PN2222 base
 *     Leave the original rocker switch ON.
 *
 *   MODE BUTTON (only if the box has a mode/timer button, via 4N35 optocoupler):
 *     Uno D7 --> 220 ohm --> 4N35 pin 1 (anode), 4N35 pin 2 --> GND
 *     4N35 pin 5 (collector) --> button leg that measures HIGHER voltage
 *     4N35 pin 4 (emitter)   --> other button leg
 *
 *   LOCAL BUTTON: one leg to D4, other leg to GND (internal pullup used)
 *   IR RECEIVER: OUT/S --> D2, VCC --> 5V, GND --> GND
 *
 * Dimming: PWM on D9 only works if the string has NO controller chip
 * (plain rocker, no modes). If the box has a chip, PWM will reset it or
 * flicker, so set ENABLE_DIMMING to false.
 *
 * Needs the "IRremote" library (v4.x) from the Library Manager.
 */

#include <IRremote.hpp>

// ---------------- pins ----------------
const int PIN_IR      = 2;
const int PIN_BUTTON  = 4;
const int PIN_MODE    = 7;   // 4N35 LED (fake button press)
const int PIN_POWER   = 9;   // PN2222 base, PWM-capable

// ---------------- settings ----------------
const bool ENABLE_IR           = false;  // true only when an IR receiver is wired to D2
const bool ENABLE_DIMMING      = true;   // false if the box has a controller chip
const unsigned long PRESS_MS   = 120;    // how long to "hold" the mode button
const unsigned long LONG_MS    = 700;    // local button long-press threshold
const unsigned long DEBOUNCE_MS = 30;
const int DIM_STEP             = 32;

// IR command codes for the common 17-key kit remote (NEC protocol).
// If yours differ, press a key and read the code printed on Serial.
const uint8_t IR_OK    = 0x1C;
const uint8_t IR_RIGHT = 0x5A;
const uint8_t IR_UP    = 0x18;
const uint8_t IR_DOWN  = 0x52;

// ---------------- state ----------------
bool lightsOn = false;
int brightness = 255;

// ---------------- outputs ----------------
void applyPower() {
  if (!lightsOn) {
    digitalWrite(PIN_POWER, LOW);
  } else if (ENABLE_DIMMING) {
    analogWrite(PIN_POWER, brightness);
  } else {
    digitalWrite(PIN_POWER, HIGH);
  }
}

void setLights(bool on) {
  lightsOn = on;
  applyPower();
  printStatus();
}

void setBrightness(int value) {
  brightness = constrain(value, 0, 255);
  applyPower();
  printStatus();
}

// Pulses the optocoupler so the original controller sees a button press.
void pressModeButton() {
  if (!lightsOn) return;  // controller has no power when off
  digitalWrite(PIN_MODE, HIGH);
  delay(PRESS_MS);
  digitalWrite(PIN_MODE, LOW);
  Serial.println(F("mode: pressed"));
}

void printStatus() {
  Serial.print(F("status: "));
  Serial.print(lightsOn ? F("on") : F("off"));
  Serial.print(F(" brightness="));
  Serial.println(brightness);
}

// ---------------- inputs ----------------
void handleSerial() {
  if (!Serial.available()) return;
  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  cmd.toLowerCase();

  if (cmd == "on")            setLights(true);
  else if (cmd == "off")      setLights(false);
  else if (cmd == "toggle")   setLights(!lightsOn);
  else if (cmd == "mode")     pressModeButton();
  else if (cmd == "status")   printStatus();
  else if (cmd.startsWith("dim ")) setBrightness(cmd.substring(4).toInt());
  else if (cmd.length() > 0) {
    Serial.print(F("unknown command: "));
    Serial.println(cmd);
  }
}

void handleIR() {
  if (!ENABLE_IR || !IrReceiver.decode()) return;

  // Ignore noise: unknown protocols decode as junk (code 0x0)
  if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
    IrReceiver.resume();
    return;
  }

  bool isRepeat = IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;
  uint8_t code = IrReceiver.decodedIRData.command;
  IrReceiver.resume();

  if (code == IR_UP)        setBrightness(brightness + DIM_STEP);  // repeats allowed
  else if (code == IR_DOWN) setBrightness(brightness - DIM_STEP);
  else if (isRepeat)        return;                                // ignore held keys
  else if (code == IR_OK)   setLights(!lightsOn);
  else if (code == IR_RIGHT) pressModeButton();
  else {
    Serial.print(F("ir: unmapped code 0x"));
    Serial.println(code, HEX);
  }
}

void handleButton() {
  static bool lastReading = HIGH;
  static bool stableState = HIGH;
  static unsigned long lastChange = 0;
  static unsigned long pressedAt = 0;

  bool reading = digitalRead(PIN_BUTTON);
  if (reading != lastReading) {
    lastChange = millis();
    lastReading = reading;
  }
  if (millis() - lastChange < DEBOUNCE_MS || reading == stableState) return;

  stableState = reading;
  if (stableState == LOW) {
    pressedAt = millis();                       // pressed
  } else {
    unsigned long held = millis() - pressedAt;  // released
    if (held >= LONG_MS) pressModeButton();
    else setLights(!lightsOn);
  }
}

// ---------------- main ----------------
void setup() {
  pinMode(PIN_POWER, OUTPUT);
  pinMode(PIN_MODE, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  digitalWrite(PIN_POWER, LOW);
  digitalWrite(PIN_MODE, LOW);

  Serial.begin(9600);
  if (ENABLE_IR) {
    IrReceiver.begin(PIN_IR, DISABLE_LED_FEEDBACK);
  }
  Serial.println(F("string lights ready"));
  printStatus();
}

void loop() {
  handleSerial();
  handleIR();
  handleButton();
}
