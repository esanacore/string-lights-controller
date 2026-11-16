/*
 * String light controller (Uno R3 + proto shield)
 *
 * The 3xAA pack is replaced by the Uno's 5V. The whole string is one
 * channel, so every effect is a brightness pattern over time.
 *
 * Control:
 *   1. Serial over USB at 9600 baud, newline-terminated:
 *        on | off | toggle | status
 *        dim <0-255>          max brightness for all effects
 *        effect <name>        solid, breathe, twinkle, candle, pulse,
 *                             blink, wave, sunrise
 *        next                 cycle to the next effect
 *        speed <1-10>         1 = slowest, 10 = fastest (default 5)
 *        list                 print effect names
 *      This is how a PC, Pi or Home Assistant drives it.
 *   2. Local push button on D4 (optional): short press = on/off,
 *      long press = next effect.
 *   3. IR remote on D2 (optional, ENABLE_IR): OK = on/off, RIGHT = next
 *      effect, UP/DOWN = brighter/dimmer.
 *
 * Wiring:
 *   Uno 5V --> 1N4007 anode, cathode (stripe) --> box + (top-left contact / switch middle lug)
 *   box - (bottom-right spring) --> 2N2222 collector
 *   2N2222 emitter --> GND
 *   Uno D9 --> 1k --> 2N2222 base
 *   Box slide switch stays ON.
 *
 * Needs the "IRremote" library (v4.x) from the Library Manager.
 */

#include <IRremote.hpp>

// ---------------- pins ----------------
const int PIN_IR     = 2;
const int PIN_BUTTON = 4;
const int PIN_POWER  = 9;   // PWM -> 2N2222 base

// ---------------- settings ----------------
const bool ENABLE_IR             = false;  // true only when an IR receiver is wired to D2
const unsigned long LONG_MS      = 700;    // button long-press threshold
const unsigned long DEBOUNCE_MS  = 30;
const int DIM_STEP               = 32;

// IR command codes for the common 17-key kit remote (NEC protocol).
const uint8_t IR_OK    = 0x1C;
const uint8_t IR_RIGHT = 0x5A;
const uint8_t IR_UP    = 0x18;
const uint8_t IR_DOWN  = 0x52;

// ---------------- effects ----------------
enum Effect { SOLID, BREATHE, TWINKLE, CANDLE, PULSE, BLINK, WAVE, SUNRISE, EFFECT_COUNT };
const char* const EFFECT_NAMES[EFFECT_COUNT] = {
  "solid", "breathe", "twinkle", "candle", "pulse", "blink", "wave", "sunrise"
};

// ---------------- state ----------------
bool lightsOn = false;
int brightness = 255;          // ceiling for every effect (0-255)
int speed = 5;                 // 1-10
Effect effect = SOLID;
unsigned long effectStart = 0; // millis() when the current effect started

// ---------------- helpers ----------------

// LEDs look linear to the eye only after a gamma curve; without this,
// breathing spends most of its time looking "fully on".
uint8_t gamma8(uint8_t x) {
  return (uint16_t)x * x / 255;
}

// Effect period in ms, scaled by speed (speed 5 = base period).
unsigned long periodMs(unsigned long base) {
  return base * 5UL / speed;
}

// Triangle wave 0..255..0 over one period.
uint8_t triangle(unsigned long t, unsigned long period) {
  unsigned long p = t % period;
  unsigned long half = period / 2;
  return p < half ? p * 255UL / half : (period - p) * 255UL / half;
}

// Returns the effect's level (0-255) before brightness and gamma.
uint8_t effectLevel(unsigned long now) {
  unsigned long t = now - effectStart;

  switch (effect) {
    case SOLID:
      return 255;

    case BREATHE: {
      // smooth sine-like breathing, never fully dark
      float phase = (t % periodMs(4000)) / (float)periodMs(4000);
      float s = (1.0 - cos(phase * 2 * PI)) / 2.0;   // 0..1
      return 20 + s * 235;
    }

    case TWINKLE: {
      // mostly bright, with random quick dips that sparkle
      static unsigned long nextChange = 0;
      static uint8_t level = 255;
      if (now >= nextChange) {
        level = random(100) < 70 ? random(200, 256) : random(40, 140);
        nextChange = now + random(30, periodMs(250));
      }
      return level;
    }

    case CANDLE: {
      // warm flicker: small random wobble around a high base, with
      // smoothing so it moves instead of jumping
      static float level = 220;
      static unsigned long last = 0;
      if (now - last >= 20) {
        last = now;
        float target = random(150, 256);
        level += (target - level) * (0.05 + speed * 0.03);
      }
      return (uint8_t)level;
    }

    case PULSE: {
      // heartbeat: two quick beats, then rest
      unsigned long p = t % periodMs(1400);
      unsigned long beat = periodMs(140);
      if (p < beat)            return 255 - p * 200 / beat;
      if (p < 2 * beat)        return 55;
      if (p < 3 * beat)        return 255 - (p - 2 * beat) * 200 / beat;
      return 55;
    }

    case BLINK:
      return (t % periodMs(1000)) < periodMs(500) ? 255 : 0;

    case WAVE:
      // slow linear fade up and down, all the way to off
      return triangle(t, periodMs(6000));

    case SUNRISE: {
      // ramps from off to full over ~10 min at speed 5, then holds
      unsigned long duration = periodMs(600000UL);
      if (t >= duration) return 255;
      return t * 255UL / duration;
    }

    default:
      return 255;
  }
}

// Called every loop: computes the output level and writes it to the transistor.
void updateOutput() {
  if (!lightsOn) {
    analogWrite(PIN_POWER, 0);
    return;
  }
  uint8_t level = (uint16_t)effectLevel(millis()) * brightness / 255;
  analogWrite(PIN_POWER, gamma8(level));
}

void printStatus() {
  Serial.print(F("status: "));
  Serial.print(lightsOn ? F("on") : F("off"));
  Serial.print(F(" effect="));
  Serial.print(EFFECT_NAMES[effect]);
  Serial.print(F(" brightness="));
  Serial.print(brightness);
  Serial.print(F(" speed="));
  Serial.println(speed);
}

void setLights(bool on) {
  lightsOn = on;
  if (on) effectStart = millis();   // effects like sunrise restart when turned on
  printStatus();
}

void setBrightness(int value) {
  brightness = constrain(value, 0, 255);
  printStatus();
}

void setEffect(Effect e) {
  effect = e;
  effectStart = millis();
  printStatus();
}

void nextEffect() {
  setEffect((Effect)((effect + 1) % EFFECT_COUNT));
}

bool setEffectByName(const String& name) {
  for (int i = 0; i < EFFECT_COUNT; i++) {
    if (name == EFFECT_NAMES[i]) {
      setEffect((Effect)i);
      return true;
    }
  }
  return false;
}

void listEffects() {
  Serial.print(F("effects:"));
  for (int i = 0; i < EFFECT_COUNT; i++) {
    Serial.print(' ');
    Serial.print(EFFECT_NAMES[i]);
  }
  Serial.println();
}

// ---------------- inputs ----------------
void handleSerial() {
  if (!Serial.available()) return;
  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  cmd.toLowerCase();

  if (cmd == "on")                    setLights(true);
  else if (cmd == "off")              setLights(false);
  else if (cmd == "toggle")           setLights(!lightsOn);
  else if (cmd == "status")           printStatus();
  else if (cmd == "next")             nextEffect();
  else if (cmd == "list")             listEffects();
  else if (cmd.startsWith("dim "))    setBrightness(cmd.substring(4).toInt());
  else if (cmd.startsWith("speed ")) {
    speed = constrain(cmd.substring(6).toInt(), 1, 10);
    printStatus();
  }
  else if (cmd.startsWith("effect ")) {
    if (!setEffectByName(cmd.substring(7))) {
      Serial.print(F("unknown effect: "));
      Serial.println(cmd.substring(7));
      listEffects();
    }
  }
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

  if (code == IR_UP)          setBrightness(brightness + DIM_STEP);  // repeats allowed
  else if (code == IR_DOWN)   setBrightness(brightness - DIM_STEP);
  else if (isRepeat)          return;                                // ignore held keys
  else if (code == IR_OK)     setLights(!lightsOn);
  else if (code == IR_RIGHT)  nextEffect();
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
    if (held >= LONG_MS) nextEffect();
    else setLights(!lightsOn);
  }
}

// ---------------- main ----------------
void setup() {
  pinMode(PIN_POWER, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  analogWrite(PIN_POWER, 0);
  randomSeed(analogRead(A0));   // floating analog pin as a noise source

  Serial.begin(9600);
  Serial.setTimeout(50);        // don't stall effects while reading a command
  if (ENABLE_IR) {
    IrReceiver.begin(PIN_IR, DISABLE_LED_FEEDBACK);
  }
  Serial.println(F("string lights ready"));
  listEffects();
  printStatus();
}

void loop() {
  handleSerial();
  handleIR();
  handleButton();
  updateOutput();
}
