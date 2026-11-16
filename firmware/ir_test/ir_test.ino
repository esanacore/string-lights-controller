/*
 * IR receiver test: watches D2 and reports how many times the signal
 * changed in each quarter second. No IR library involved.
 *
 * Expected: "idle" while nothing happens, then a burst of changes
 * (dozens or more) whenever a remote button is pressed at the receiver.
 */

const int PIN_IR = 2;

void setup() {
  pinMode(PIN_IR, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println(F("IR test ready: press remote buttons at the receiver"));
}

void loop() {
  unsigned long start = millis();
  unsigned int changes = 0;
  int last = digitalRead(PIN_IR);
  int lowSeen = 0;

  // sample as fast as possible for 250 ms
  while (millis() - start < 250) {
    int now = digitalRead(PIN_IR);
    if (now != last) { changes++; last = now; }
    if (now == LOW) lowSeen = 1;
  }

  if (changes > 0) {
    Serial.print(F("activity: "));
    Serial.print(changes);
    Serial.println(F(" changes"));
  } else if (lowSeen) {
    Serial.println(F("pin stuck LOW"));
  } else {
    static unsigned long lastIdle = 0;
    if (millis() - lastIdle > 2000) {   // say "idle" every 2 s so you know it's alive
      Serial.println(F("idle"));
      lastIdle = millis();
    }
  }
}
