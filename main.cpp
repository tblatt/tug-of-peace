#include <FastLED.h>

#define LED_PIN_1 18
#define LED_PIN_2 19
#define NUM_LEDS_1 64
#define NUM_LEDS_2 105
#define BUTTON_PIN 26
#define BUTTON2_PIN 25

uint8_t gHue = 0; 
CRGB leds1[NUM_LEDS_1];
CRGB leds2[NUM_LEDS_2];

int ledIndex = NUM_LEDS_1 / 2;  
int ledWidth = 10;  
unsigned long buttonPressTime = 0;
unsigned long buttonReleaseTime = 0;
unsigned long debounce = 10;  
bool buttonPressed = false;
bool gameMode = false;
unsigned long button2PressTime = 0;
unsigned long button2ReleaseTime = 0;
bool button2Pressed = false;
bool blueReady = false;  // Tracks if the blue player is ready
bool yellowReady = false;  // Tracks if the yellow player is ready
const int LED_OFFSET_2 = 2; 
int colorIndex = 0;

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN_1, GRB>(leds1, NUM_LEDS_1);
  FastLED.addLeds<WS2812B, LED_PIN_2, GRB>(leds2, NUM_LEDS_2);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  FastLED.clear();
  FastLED.show();
  Serial.begin(115200);
}

void attractMode() {
  fill_rainbow(leds1, NUM_LEDS_1, gHue, 7);
  fill_rainbow(leds2, NUM_LEDS_2, gHue, 7);
  FastLED.show();
  EVERY_N_MILLISECONDS(10) { gHue++; }
}



void loop() {
  if (!gameMode) {
    // Only run attractMode if neither player is ready
    if (!blueReady && !yellowReady) {
            FastLED.setBrightness(128);

      attractMode();
    }

    // Check if blue button is pressed
    if (digitalRead(BUTTON_PIN) == LOW && !blueReady) {
      blueReady = true;
      delay(200);  // Debounce delay
    }

    // Check if yellow button is pressed
    if (digitalRead(BUTTON2_PIN) == LOW && !yellowReady) {
      yellowReady = true;
      delay(200);  // Debounce delay
    }



//  Serial.print("BUTTON_PIN: ");
//  Serial.print(digitalRead(BUTTON_PIN));
//  Serial.print(" BUTTON2_PIN: ");
//  Serial.println(digitalRead(BUTTON2_PIN));

    // If both players are ready, start the game
    if (blueReady && yellowReady) {
      gameMode = true;  // Enter game mode
      ledIndex = NUM_LEDS_1 / 2;  // Reset LED index to center
      blueReady = false;  // Reset readiness for the next game
      yellowReady = false;
      FastLED.clear();
          FastLED.setBrightness(128);

      FastLED.show();
      
    }
  } else {
    colorIndex++;  // Increment for animation
            
    // Clear all LEDs before updating
    FastLED.clear();
            
    // Update the moving LEDs
    for (int i = -ledWidth / 2; i <= ledWidth / 2; i++) {
      int index1 = constrain(ledIndex + i, 0, NUM_LEDS_1 - 1);
      int mappedIndex2 = map(index1, 0, NUM_LEDS_1 - 1, 0, NUM_LEDS_2 - 1) + LED_OFFSET_2;
            
      if (mappedIndex2 >= 0 && mappedIndex2 < NUM_LEDS_2) {
        leds1[index1] = Wheel(colorIndex + 20 * index1);
        leds2[mappedIndex2] = leds1[index1];
      }
    }
            
    // Show updated LED state
    FastLED.show();

    if (digitalRead(BUTTON_PIN) == LOW) {
      if (!buttonPressed) {
        buttonPressTime = millis();
        if (buttonPressTime - buttonReleaseTime >= debounce) {
          ledIndex = max(0, ledIndex - 1);
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
          ledIndex = min(NUM_LEDS_1 - 1, ledIndex + 1);
          button2Pressed = true;
        }
      }
    } else {
      button2ReleaseTime = millis();
      button2Pressed = false;
    }

    if (ledIndex - ledWidth / 2 <= 0 || ledIndex + ledWidth / 2 >= NUM_LEDS_1) {
      handleGameEnd();
    }
  }
}
void handleGameEnd() {
  if (ledIndex - ledWidth / 2 <= 0) {
    fill_solid(leds1, NUM_LEDS_1, CRGB::Blue);
    fill_solid(leds2, NUM_LEDS_2, CRGB::Blue);
  } else {
    FastLED.setBrightness(64);
    fill_solid(leds1, NUM_LEDS_1, CRGB::Yellow);
    fill_solid(leds2, NUM_LEDS_2, CRGB::Yellow);
  }
  FastLED.show();
  delay(2000);
  FastLED.clear();
  FastLED.show();

  ledIndex = NUM_LEDS_1 / 2;
  gameMode = false;
  blueReady = false;
  yellowReady = false;


}

CRGB Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) return CRGB(255 - WheelPos * 3, 0, WheelPos * 3);
  if (WheelPos < 170) {
    WheelPos -= 85;
    return CRGB(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
}
