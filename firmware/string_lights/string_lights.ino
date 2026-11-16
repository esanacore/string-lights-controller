/*
 * String light controller v2 (Uno R3 + proto shield)
 *
 * New in v2:
 *   - remembers on/off, effect, brightness and speed across power cuts (EEPROM)
 *   - IR remote (kit receiver on D2)
 *   - two buttons: S1 on the shield (D4) and a second button (D5)
 *   - MAX7219 8x8 LED matrix shows an icon for the current effect
 *
 * Pins:
 *   D2   IR receiver OUT        (receiver VCC -> 5V, GND -> GND)
 *   D4   Button 1 (shield S1)   other leg -> GND
 *   D5   Button 2               other leg -> GND
 *   D9   PWM -> 1k -> 2N2222 base (string lights)
 *   D10  MAX7219 CS
 *   D11  MAX7219 CLK
 *   D12  MAX7219 DIN            (matrix VCC -> 5V, GND -> GND)
 *
 * Power-up (CYCLE_ON_POWERUP): every time it gets power, the lights come on at
 * full brightness on the next effect in the list. Set it to false to make it
 * resume exactly where it left off instead.
 *
 * Buttons:
 *   Button 1: short = on/off, long = next effect
 *   Button 2: short = next brightness step, long = next speed step
 *
 * IR (21-key kit remote):
 *   Power = on/off, Mode or Next = next effect, Prev = previous effect,
 *   + / - = brighter/dimmer (hold to repeat), EQ = faster, USB SCAN = slower,
 *   1-8 = jump to effect, 9 = full brightness, 0 = night mode,
 *   Play/Pause = freeze effect, Mute = matrix display on/off.
 *   Unknown keys print their code on Serial so they can be mapped below.
 *
 * Serial (9600, newline): on | off | toggle | status | list | next
 *   dim <0-255> | speed <1-10> | effect <name>
 *
 * Libraries (Library Manager): "IRremote" by Armin Joachimsmeyer,
 *                              "LedControl" by Eberhard Fahle
 */

#include <IRremote.hpp>
#include <LedControl.h>
#include <EEPROM.h>

// ---------------- pins ----------------
const int PIN_IR      = 2;
const int PIN_BTN1    = 4;
const int PIN_BTN2    = 5;
const int PIN_POWER   = 9;
const int PIN_MX_CS   = 10;
const int PIN_MX_CLK  = 11;
const int PIN_MX_DIN  = 12;

// ---------------- settings ----------------
const bool ENABLE_IR        = true;
const bool ENABLE_MATRIX    = true;
const bool CYCLE_ON_POWERUP = true;   // each power-up: lights on, full brightness, next effect
const int  MATRIX_INTENSITY = 1;
const int  MATRIX_ROTATE    = 1;      // quarter turns clockwise (0-3) to make icons upright
const bool MATRIX_MIRROR    = false;  // flip left/right if icons come out mirrored      // 0-15; keep low, the matrix shares USB power with the string
const unsigned long LONG_MS     = 700;
const unsigned long DEBOUNCE_MS = 30;
const unsigned long SAVE_DELAY_MS = 3000;   // wait for changes to settle before writing EEPROM
const unsigned long OVERLAY_MS  = 1500;     // how long brightness/speed bars stay on the matrix
const int DIM_STEP = 32;
const uint8_t BRIGHT_STEPS[] = {255, 128, 48, 16};

// IR command codes (NEC) for the 21-key kit remote. Fix any that print as "unmapped".
const uint8_t IR_POWER = 0x45, IR_MODE = 0x46, IR_MUTE = 0x47;
const uint8_t IR_PLAY  = 0x44, IR_PREV = 0x40, IR_NEXT = 0x43;
const uint8_t IR_EQ    = 0x07, IR_MINUS = 0x15, IR_PLUS = 0x09;
const uint8_t IR_ZERO  = 0x16, IR_LOOP = 0x19, IR_SCAN = 0x0D;
const uint8_t IR_DIGITS[9] = {0x0C, 0x18, 0x5E, 0x08, 0x1C, 0x5A, 0x42, 0x52, 0x4A};  // keys 1-9
const uint8_t NIGHT_BRIGHTNESS = 16;

// ---------------- effects ----------------
enum Effect { SOLID, BREATHE, TWINKLE, CANDLE, PULSE, BLINK, WAVE, SUNRISE, EFFECT_COUNT };
const char* const EFFECT_NAMES[EFFECT_COUNT] = {
  "solid", "breathe", "twinkle", "candle", "pulse", "blink", "wave", "sunrise"
};

// 8x8 icons, one byte per row, top row first, MSB = left column
const uint8_t ICONS[EFFECT_COUNT][8] PROGMEM = {
  {0x00, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x00},  // solid: filled square
  {0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00},  // breathe: ring
  {0x10, 0x10, 0x7C, 0x10, 0x10, 0x02, 0x07, 0x02},  // twinkle: two sparkles
  {0x08, 0x18, 0x1C, 0x3C, 0x3E, 0x3E, 0x1C, 0x08},  // candle: flame
  {0x00, 0x66, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00},  // pulse: heart
  {0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F},  // blink: checker blocks
  {0x00, 0x00, 0x30, 0x48, 0x84, 0x02, 0x01, 0x00},  // wave: curve
  {0x00, 0x00, 0x00, 0x18, 0x3C, 0x7E, 0xFF, 0x00},  // sunrise: rising half sun
};

// ---------------- state ----------------
bool lightsOn = false;
int brightness = 255;
int speed = 5;
Effect effect = SOLID;
unsigned long effectStart = 0;

bool saveDue = false;
unsigned long lastChange = 0;

unsigned long overlayUntil = 0;   // matrix shows a bar instead of the icon until this time
uint8_t overlayValue = 0;         // 0-8 bar length
bool matrixDirty = true;
bool matrixOn = true;             // MUTE key toggles the display

bool paused = false;              // PLAY key freezes the effect where it is
unsigned long pausedAt = 0;

LedControl matrix(PIN_MX_DIN, PIN_MX_CLK, PIN_MX_CS, 1);

// ---------------- persistence ----------------
const uint8_t EEPROM_MAGIC = 0xA7;   // changes if the saved layout ever changes
struct Saved { uint8_t magic, on, effect, brightness, speed; };

void loadState() {
  Saved s;
  EEPROM.get(0, s);
  if (s.magic != EEPROM_MAGIC || s.effect >= EFFECT_COUNT || s.speed < 1 || s.speed > 10) return;
  lightsOn = s.on;
  effect = (Effect)s.effect;
  brightness = s.brightness;
  speed = s.speed;
}

void saveState() {
  Saved s = {EEPROM_MAGIC, (uint8_t)lightsOn, (uint8_t)effect, (uint8_t)brightness, (uint8_t)speed};
  EEPROM.put(0, s);   // put() only writes bytes that changed, which spares the EEPROM
}

void printStatus();

// Every change goes through here: prints status, redraws, and schedules a save.
void changed() {
  printStatus();
  matrixDirty = true;
  saveDue = true;
  lastChange = millis();
}

// ---------------- helpers ----------------
uint8_t gamma8(uint8_t x) { return (uint16_t)x * x / 255; }
unsigned long periodMs(unsigned long base) { return base * 5UL / speed; }

uint8_t triangle(unsigned long t, unsigned long period) {
  unsigned long p = t % period, half = period / 2;
  return p < half ? p * 255UL / half : (period - p) * 255UL / half;
}

uint8_t effectLevel(unsigned long now) {
  unsigned long t = now - effectStart;
  switch (effect) {
    case SOLID: return 255;
    case BREATHE: {
      float phase = (t % periodMs(4000)) / (float)periodMs(4000);
      return 20 + (1.0 - cos(phase * 2 * PI)) / 2.0 * 235;
    }
    case TWINKLE: {
      static unsigned long next = 0;
      static uint8_t level = 255;
      if (now >= next) {
        level = random(100) < 70 ? random(200, 256) : random(40, 140);
        next = now + random(30, periodMs(250));
      }
      return level;
    }
    case CANDLE: {
      static float level = 220;
      static unsigned long last = 0;
      if (now - last >= 20) {
        last = now;
        level += (random(150, 256) - level) * (0.05 + speed * 0.03);
      }
      return (uint8_t)level;
    }
    case PULSE: {
      unsigned long p = t % periodMs(1400), beat = periodMs(140);
      if (p < beat)     return 255 - p * 200 / beat;
      if (p < 2 * beat) return 55;
      if (p < 3 * beat) return 255 - (p - 2 * beat) * 200 / beat;
      return 55;
    }
    case BLINK:   return (t % periodMs(1000)) < periodMs(500) ? 255 : 0;
    case WAVE:    return triangle(t, periodMs(6000));
    case SUNRISE: {
      unsigned long duration = periodMs(30000UL);   // ~30 s ramp at speed 5, then holds
      return t >= duration ? 255 : t * 255UL / duration;
    }
    default: return 255;
  }
}

void updateOutput() {
  if (!lightsOn) { analogWrite(PIN_POWER, 0); return; }
  unsigned long now = paused ? pausedAt : millis();
  uint8_t level = (uint16_t)effectLevel(now) * brightness / 255;
  analogWrite(PIN_POWER, gamma8(level));
}

// ---------------- matrix ----------------
void showOverlay(uint8_t value0to8) {
  overlayValue = value0to8;
  overlayUntil = millis() + OVERLAY_MS;
  matrixDirty = true;
}

// Sends an upright 8x8 picture to the matrix, applying MATRIX_ROTATE / MATRIX_MIRROR
// to cancel out how the module is wired.
void drawImage(const uint8_t img[8]) {
  uint8_t out[8] = {0};
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 8; c++) {
      if (!(img[r] & (0x80 >> c))) continue;
      int cc = MATRIX_MIRROR ? 7 - c : c;
      int nr = r, nc = cc;
      for (int q = 0; q < MATRIX_ROTATE; q++) {   // rotate 90 degrees clockwise per step
        int t = nr; nr = nc; nc = 7 - t;
      }
      out[nr] |= 0x80 >> nc;
    }
  }
  for (int r = 0; r < 8; r++) matrix.setRow(0, r, out[r]);
}

void updateMatrix() {
  if (!ENABLE_MATRIX || !matrixOn) return;

  // the overlay timing out also needs a redraw
  static bool overlayWasOn = false;
  bool overlayOn = millis() < overlayUntil;
  if (overlayOn != overlayWasOn) { overlayWasOn = overlayOn; matrixDirty = true; }
  if (!matrixDirty) return;
  matrixDirty = false;

  if (!lightsOn && !overlayOn) {
    // off: one dim dot in the corner so you can tell the box has power
    uint8_t dot[8] = {0, 0, 0, 0, 0, 0, 0, 0x01};   // bottom-right corner
    drawImage(dot);
    return;
  }

  // build the picture upright (row 0 = top, MSB = left)
  uint8_t img[8];
  for (int row = 0; row < 8; row++) {
    if (overlayOn) {
      // horizontal bar graph across the bottom two rows
      img[row] = (row >= 6) ? (uint8_t)(0xFF << (8 - overlayValue)) : 0;
    } else {
      img[row] = pgm_read_byte(&ICONS[effect][row]);
    }
  }
  drawImage(img);
}

// ---------------- actions ----------------
void setLights(bool on) {
  lightsOn = on;
  if (on) effectStart = millis();
  changed();
}

void setBrightness(int value) {
  brightness = constrain(value, 0, 255);
  showOverlay((brightness * 8 + 254) / 255);
  changed();
}

void setSpeed(int value) {
  speed = constrain(value, 1, 10);
  showOverlay((speed * 8 + 9) / 10);
  changed();
}

void setEffect(Effect e) {
  effect = e;
  effectStart = millis();
  paused = false;
  overlayUntil = 0;   // show the new icon right away
  changed();
}

void togglePause() {
  if (paused) effectStart += millis() - pausedAt;   // resume where it left off
  else pausedAt = millis();
  paused = !paused;
  Serial.println(paused ? F("effect paused") : F("effect resumed"));
}

void toggleMatrix() {
  matrixOn = !matrixOn;
  if (ENABLE_MATRIX) matrix.shutdown(0, !matrixOn);
  matrixDirty = true;
}

void nextEffect(int dir) {
  setEffect((Effect)((effect + EFFECT_COUNT + dir) % EFFECT_COUNT));
}

void nextBrightnessStep() {
  // jump to the next preset below the current level, wrapping to full
  for (uint8_t step : BRIGHT_STEPS) {
    if (step < brightness) { setBrightness(step); return; }
  }
  setBrightness(BRIGHT_STEPS[0]);
}

void printStatus() {
  Serial.print(F("status: "));
  Serial.print(lightsOn ? F("on") : F("off"));
  Serial.print(F(" effect="));     Serial.print(EFFECT_NAMES[effect]);
  Serial.print(F(" brightness=")); Serial.print(brightness);
  Serial.print(F(" speed="));      Serial.println(speed);
}

void listEffects() {
  Serial.print(F("effects:"));
  for (int i = 0; i < EFFECT_COUNT; i++) { Serial.print(' '); Serial.print(EFFECT_NAMES[i]); }
  Serial.println();
}

// ---------------- inputs ----------------
void handleSerial() {
  if (!Serial.available()) return;
  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  cmd.toLowerCase();

  if (cmd == "on")                   setLights(true);
  else if (cmd == "off")             setLights(false);
  else if (cmd == "toggle")          setLights(!lightsOn);
  else if (cmd == "status")          printStatus();
  else if (cmd == "list")            listEffects();
  else if (cmd == "next")            nextEffect(1);
  else if (cmd.startsWith("dim "))   setBrightness(cmd.substring(4).toInt());
  else if (cmd.startsWith("speed ")) setSpeed(cmd.substring(6).toInt());
  else if (cmd.startsWith("effect ")) {
    String name = cmd.substring(7);
    for (int i = 0; i < EFFECT_COUNT; i++) {
      if (name == EFFECT_NAMES[i]) { setEffect((Effect)i); return; }
    }
    Serial.print(F("unknown effect: ")); Serial.println(name);
    listEffects();
  }
  else if (cmd.length() > 0) { Serial.print(F("unknown command: ")); Serial.println(cmd); }
}

void handleIR() {
  if (!ENABLE_IR || !IrReceiver.decode()) return;

  if (IrReceiver.decodedIRData.protocol == UNKNOWN) {   // noise
    IrReceiver.resume();
    return;
  }
  bool isRepeat = IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;
  uint8_t code = IrReceiver.decodedIRData.command;
  IrReceiver.resume();

  // holding +/- keeps changing brightness
  if (code == IR_PLUS)  { setBrightness(brightness + DIM_STEP); return; }
  if (code == IR_MINUS) { setBrightness(brightness - DIM_STEP); return; }
  if (isRepeat) return;

  if (code == IR_POWER)                       setLights(!lightsOn);
  else if (code == IR_MODE || code == IR_NEXT) nextEffect(1);
  else if (code == IR_PREV)                   nextEffect(-1);
  else if (code == IR_EQ)                     setSpeed(speed + 1);
  else if (code == IR_SCAN)                   setSpeed(speed - 1);
  else if (code == IR_PLAY)                   togglePause();
  else if (code == IR_MUTE)                   toggleMatrix();
  else if (code == IR_ZERO)                   setBrightness(NIGHT_BRIGHTNESS);
  else if (code == IR_DIGITS[8])              setBrightness(255);   // key 9
  else {
    for (int i = 0; i < EFFECT_COUNT; i++) {   // keys 1-8 pick an effect
      if (code == IR_DIGITS[i]) {
        if (!lightsOn) setLights(true);
        setEffect((Effect)i);
        return;
      }
    }
    Serial.print(F("ir: unmapped code 0x")); Serial.println(code, HEX);
  }
}

// Debounced button with short/long press. Returns 0 = nothing, 1 = short, 2 = long.
struct Button {
  int pin;
  bool lastReading, stable;
  unsigned long lastFlip, pressedAt;

  int poll() {
    bool reading = digitalRead(pin);
    if (reading != lastReading) { lastFlip = millis(); lastReading = reading; }
    if (millis() - lastFlip < DEBOUNCE_MS || reading == stable) return 0;
    stable = reading;
    if (stable == LOW) { pressedAt = millis(); return 0; }   // pressed
    return (millis() - pressedAt >= LONG_MS) ? 2 : 1;        // released
  }
};

Button btn1 = {PIN_BTN1, HIGH, HIGH, 0, 0};
Button btn2 = {PIN_BTN2, HIGH, HIGH, 0, 0};

void handleButtons() {
  switch (btn1.poll()) {
    case 1: setLights(!lightsOn); break;
    case 2: nextEffect(1); break;
  }
  switch (btn2.poll()) {
    case 1: nextBrightnessStep(); break;
    case 2: setSpeed(speed % 10 + 1); break;   // 1..10 then wraps
  }
}

// ---------------- main ----------------
void setup() {
  pinMode(PIN_POWER, OUTPUT);
  pinMode(PIN_BTN1, INPUT_PULLUP);
  pinMode(PIN_BTN2, INPUT_PULLUP);
  analogWrite(PIN_POWER, 0);
  randomSeed(analogRead(A0));

  Serial.begin(9600);
  Serial.setTimeout(50);

  if (ENABLE_IR) {
    IrReceiver.begin(PIN_IR, DISABLE_LED_FEEDBACK);
    pinMode(PIN_IR, INPUT_PULLUP);   // keeps D2 quiet if the receiver is unplugged
  }
  if (ENABLE_MATRIX) {
    matrix.shutdown(0, false);
    matrix.setIntensity(0, MATRIX_INTENSITY);
    matrix.clearDisplay(0);
  }

  loadState();
  if (CYCLE_ON_POWERUP) {
    // unplug/replug to step through effects without a remote
    lightsOn = true;
    brightness = 255;
    effect = (Effect)((effect + 1) % EFFECT_COUNT);
    saveState();   // save now so a quick replug still advances
  }
  effectStart = millis();

  Serial.println(F("string lights v2 ready"));
  listEffects();
  printStatus();
}

void loop() {
  handleSerial();
  handleIR();
  handleButtons();
  updateOutput();
  updateMatrix();

  if (saveDue && millis() - lastChange >= SAVE_DELAY_MS) {
    saveState();
    saveDue = false;
  }
}
