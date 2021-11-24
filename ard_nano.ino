/*
  MIT License

  Copyright (c) 2021 ntfshard

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

constexpr int32_t hoursBeforeReset = 14;
constexpr int32_t secondsBeforeReset = 60 * 60 * hoursBeforeReset;

static int32_t secondsAfterUpdate;

const char * const welcomeMsg = R"foo(Input: [RrYyGgBbWwAa]
 * First letter of a colour is a command. Capital to switch-on. Lower-case to turn-down.
 * [Aa] - All)foo";

enum leds : int {
  redLed = 12,
  yelLed = 10,
  grnLed = 8,
  bluLed = 6,
  whtLed = 4
};

void showHelp() {
  Serial.println(welcomeMsg);
  Serial.print("State become all_lights_turn-down after approx. ");
  Serial.print(hoursBeforeReset, DEC);
  Serial.println(" hour from the last command");
}

auto pinState(const bool state) -> decltype(LOW) {
  return state ? LOW : HIGH; // inverse logic here, pull down pin to switch-on
}

void ledLit(const leds led, const bool state) {
  secondsAfterUpdate = secondsBeforeReset;
  digitalWrite(led, pinState(state));
}

void ledAll(const bool state) {
  ledLit(redLed, state);
  ledLit(yelLed, state);
  ledLit(grnLed, state);
  ledLit(bluLed, state);
  ledLit(whtLed, state);
}

void setup() {
  Serial.begin(9600);
  secondsAfterUpdate = 0;

  ledAll(true);
  pinMode(redLed, OUTPUT);
  pinMode(yelLed, OUTPUT);
  pinMode(grnLed, OUTPUT);
  pinMode(bluLed, OUTPUT);
  pinMode(whtLed, OUTPUT);
  delay(768);
  ledAll(false);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (secondsAfterUpdate) {
    --secondsAfterUpdate;
    if (0 == secondsAfterUpdate) {
      ledAll(false);
    }
  }
  delay(1000);
}

void serialEvent() {
  static bool blinkEmbedLed;
  digitalWrite(LED_BUILTIN, blinkEmbedLed ? LOW : HIGH);
  blinkEmbedLed = !blinkEmbedLed;

  while (Serial.available()) {
    const char inChar = static_cast<char>(Serial.read());
    switch (inChar) {
      case 'R': ledLit(redLed, true); break;
      case 'r': ledLit(redLed, false); break;
      case 'Y': ledLit(yelLed, true); break;
      case 'y': ledLit(yelLed, false); break;
      case 'G': ledLit(grnLed, true); break;
      case 'g': ledLit(grnLed, false); break;
      case 'B': ledLit(bluLed, true); break;
      case 'b': ledLit(bluLed, false); break;
      case 'W': ledLit(whtLed, true); break;
      case 'w': ledLit(whtLed, false); break;
      case 'A': ledAll(true); break;
      case 'a': ledAll(false); break;
      case 'h': showHelp(); break;
      default: break;
    }
  }
}
