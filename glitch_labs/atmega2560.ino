// car hacking village ph
// turn your Arduino mega 2560 into a chip for practicing fault injection
// from @shipcod3 with luv <3

#include <Arduino.h>

// Pins
static const uint8_t FLAG_LED   = 13;   // on‑board LED
static const uint8_t GLITCH_PIN = 7;    // D7 watched for glitches (INPUT_PULLUP), but you can target the main chip (better PoC)

// Glitch window in microseconds (tweak as needed)
static const uint16_t MIN_US = 1;     // accept pulses >= 1 µs
static const uint16_t MAX_US = 2000;  // and <= 2000 µs (2 ms)

volatile bool flagged = false;

// Convert Timer1 ticks to microseconds.
// Timer1 @ prescaler=8 => 2 MHz => 0.5 µs per tick (good tradeoff)
static inline void timer1_init() {
  TCCR1A = 0;                // normal mode
  TCCR1B = 0;
  TCNT1  = 0;
  TCCR1B |= (1 << CS11);     // prescaler = 8
}

static inline uint16_t now_ticks() { return TCNT1; }
static inline uint16_t ticks_to_us(uint16_t t) { return t / 2; } // 0.5 µs per tick

void setup() {
  pinMode(FLAG_LED, OUTPUT);
  digitalWrite(FLAG_LED, LOW);

  pinMode(GLITCH_PIN, INPUT_PULLUP);  // idle HIGH
  timer1_init();

  Serial.begin(115200);
  delay(50);
  Serial.println(F("-= Glitch PoC by CHV PH =- \n"));
  Serial.println(F("-= Glitch‑Flag armed. Glitch Me Po =- \n"));
  Serial.print(F("Accepting pulses in ["));
  Serial.print(MIN_US); Serial.print(F(", "));
  Serial.print(MAX_US); Serial.println(F("] µs"));
}

void loop() {
  if (!flagged) {
    // Wait for falling edge (HIGH -> LOW)
    if (digitalRead(GLITCH_PIN) == LOW) {
      uint16_t t0 = now_ticks();

      // Stay here while LOW to measure pulse width
      while (digitalRead(GLITCH_PIN) == LOW) {
        // Optional: timeout in case of stuck LOW
        if (ticks_to_us(now_ticks() - t0) > (MAX_US + 100)) break;
      }

      uint16_t dt_us = ticks_to_us(now_ticks() - t0);

      if (dt_us >= MIN_US && dt_us <= MAX_US) {
        flagged = true;
        digitalWrite(FLAG_LED, HIGH);
        Serial.println(F("[+] FLAG{put your sugar on me}"));
        Serial.print(F("Pulse width: ")); Serial.print(dt_us); Serial.println(F(" µs"));
      }
      // Debounce/minimal guard; small delay to avoid re‑trigger spam
      delay(2);
    }
  } else {
    // After flag: idle with LED on. Hold D7 LOW > 100 ms to reset.
    if (digitalRead(GLITCH_PIN) == LOW) {
      delay(120); // long hold to confirm intent
      if (digitalRead(GLITCH_PIN) == LOW) {
        flagged = false;
        digitalWrite(FLAG_LED, LOW);
        Serial.println(F("[*] Reset. Re‑armed."));
        delay(50);
      }
    }
  }
}
