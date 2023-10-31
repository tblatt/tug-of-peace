#include <FastLED.h>

#define LED_PIN 13  // Pin the LED strip is connected to
#define NUM_LEDS 64  // Number of LEDs in the strip
#define BUTTON_PIN 16  // Pin the blue button is connected to
#define BUTTON2_PIN 17  // Pin the yellow button is connected to

CRGB leds[NUM_LEDS];  // Define an array of CRGB for your LEDs
int ledIndex = NUM_LEDS / 2;  // Keeps track of the current LED
int ledWidth = 14;  // width of the led rainbow snake that moves 
unsigned long buttonPressTime = 0;
unsigned long buttonReleaseTime = 0;
unsigned long debounce = 10;  // Debounce time in milliseconds
bool buttonPressed = false;
int colorIndex = 0;
bool gameMode = false;  // Flag to indicate game mode vs attract reel

unsigned long button2PressTime = 0;
unsigned long button2ReleaseTime = 0;
bool button2Pressed = false;

CRGB Wheel(byte WheelPos);

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  FastLED.clear();
  FastLED.show();
}

void attractMode() {
  // attract mode animation goes until someone holds a button
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Green;
    FastLED.show();
    delay(50);
  }
  FastLED.clear();
  FastLED.show();
  delay(1000);
}


void loop() {

  if (!gameMode) {
    attractMode();
    if (digitalRead(BUTTON_PIN) == LOW || digitalRead(BUTTON2_PIN) == LOW) {
      gameMode = true;  // Transition to game mode when a button is pressed
   for  (int i = 0; i < NUM_LEDS; i++) {  //  this is the play once animation for game start like ready set go
    leds[i] = CRGB::Red;
    FastLED.show();
    delay(20);
  }
  FastLED.clear();
  FastLED.show();
  delay(1000);
   
   }
   } else {

   // gameStart();
  colorIndex++;
  for (int i = ledIndex - ledWidth/2; i <= ledIndex + ledWidth/2; i++) {
    leds[i] = Wheel(colorIndex + 20 * i);
  }
  FastLED.show();

  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!buttonPressed) {
      buttonPressTime = millis();
      if (buttonPressTime - buttonReleaseTime >= debounce) {
        ledIndex--;
        FastLED.clear();
        FastLED.show();
        buttonPressed = true;
      }
    }
  } else {
    buttonReleaseTime = millis();
    buttonPressed = false;
  }

  if (digitalRead(BUTTON2_PIN) == LOW) {
    if (!button2Pressed) {
      button2PressTime = millis();
      if (button2PressTime - button2ReleaseTime >= debounce) {
        ledIndex++;
        FastLED.clear();
        FastLED.show();
        button2Pressed = true;
      }
    }
  } else {
    button2ReleaseTime = millis();
    button2Pressed = false;
  }

  if (ledIndex - ledWidth/2 <= 0 || ledIndex + ledWidth/2 >= NUM_LEDS) {
    if (ledIndex - ledWidth/2 <= 0) {  // blue wins
      // Set the entire strip to blue
      fill_solid(leds, NUM_LEDS, CRGB::Blue);
      FastLED.show();
      

      // Animation: Blue wins
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = Wheel(85 + (240 / NUM_LEDS) * (-i));
        FastLED.show();
        delay(75);
      }
      delay(2000);
    } else {  // yellow wins
      // Set the entire strip to yellow
      fill_solid(leds, NUM_LEDS, CRGB::Yellow);
      FastLED.show();
      

      // Animation: Yellow wins
      for (int i = NUM_LEDS - 1; i >= 0; i--) {
        leds[i] = Wheel(145 + (240 / NUM_LEDS) * i);
        FastLED.show();
        delay(75);
      }
      delay(2000);
    }
    FastLED.clear();
    ledIndex = NUM_LEDS / 2;
    FastLED.show();
    delay(2000);
    gameMode = false; // Return to attract mode
  }
}
}

CRGB Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return CRGB(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return CRGB(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
}