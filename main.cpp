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
const int END_OFFSET = 5;
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
const int LED_OFFSET_2 = -2; 
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
  // Only run rainbow effect if neither player is ready
  int readyLength1 = NUM_LEDS_1 / 3;  // First 1/3 of strip 1
  int readyLength2 = NUM_LEDS_2 / 3;  // First 1/3 of strip 2

  if (!blueReady && !yellowReady) {
    // If no player is ready, continue the attract mode with full rainbow effect
    fill_rainbow(leds1, NUM_LEDS_1, gHue, 7);
    fill_rainbow(leds2, NUM_LEDS_2, gHue, 7);
    EVERY_N_MILLISECONDS(10) { gHue++; } // Increment rainbow hue every 10ms
  } else {
    // If blue player is ready, show blue on first 1/3 of both strips, rainbow on the rest
    if (blueReady && !yellowReady) {
      fill_solid(leds1, readyLength1, CRGB::Blue);  // First 1/3 of strip 1
      fill_rainbow(&leds1[readyLength1], NUM_LEDS_1 - readyLength1, gHue, 7);  // Rest of strip 1

      fill_solid(leds2, readyLength2, CRGB::Blue);  // First 1/3 of strip 2
      fill_rainbow(&leds2[readyLength2], NUM_LEDS_2 - readyLength2, gHue, 7);  // Rest of strip 2
    }
    // If yellow player is ready, show yellow on first 1/3 of both strips, rainbow on the rest
    else if (yellowReady && !blueReady) {
      fill_solid(leds1, readyLength1, CRGB::Yellow);  // First 1/3 of strip 1
      fill_rainbow(&leds1[readyLength1], NUM_LEDS_1 - readyLength1, gHue, 7);  // Rest of strip 1

      fill_solid(leds2, readyLength2, CRGB::Yellow);  // First 1/3 of strip 2
      fill_rainbow(&leds2[readyLength2], NUM_LEDS_2 - readyLength2, gHue, 7);  // Rest of strip 2
    }
  }

  EVERY_N_MILLISECONDS(15) { gHue++; } // Increment rainbow hue every 10ms

  FastLED.show();
}

void transitionToBlack() {
  int middle1 = NUM_LEDS_1 / 2;
  int middle2 = NUM_LEDS_2 / 2;
  float ratio = float(NUM_LEDS_2) / float(NUM_LEDS_1); // Ratio of lengths between strips

  for (int offset = 0; offset <= middle1; offset++) {
    // Calculate corresponding offset for the second strip
    int offset2 = int(offset * ratio);

    // Gradually turn off LEDs from the center outward on the first strip
    if (middle1 - offset >= 0) leds1[middle1 - offset] = CRGB::Black;
    if (middle1 + offset < NUM_LEDS_1) leds1[middle1 + offset] = CRGB::Black;

    // Gradually turn off LEDs from the center outward on the second strip
    for (int i = 0; i <= offset2; i++) {
      if (middle2 - i >= 0) leds2[middle2 - i] = CRGB::Black;
      if (middle2 + i < NUM_LEDS_2) leds2[middle2 + i] = CRGB::Black;
    }


    FastLED.show();
    delay(35); // Adjust for animation speed
  }
  delay(750);
}

void loop() {
  if (!gameMode) {
    // Only run attractMode if neither player is ready
    FastLED.setBrightness(255);

    int readyLength1 = NUM_LEDS_1 / 3;  // 1/3 of strip 1
    int readyLength2 = NUM_LEDS_2 / 3;  // 1/3 of strip 2

    // If no player is ready, continue the attract mode
    if (!blueReady && !yellowReady) {
      attractMode();
    }

    // Check if blue button is pressed
    if (digitalRead(BUTTON_PIN) == LOW && !blueReady) {
      blueReady = true;
      delay(100);  // Debounce delay
    }

    // Check if yellow button is pressed
    if (digitalRead(BUTTON2_PIN) == LOW && !yellowReady) {
      yellowReady = true;
      delay(100);  // Debounce delay
    }

    // If blue player is ready, show blue on first 1/3 of both strips, rainbow on the rest
    if (blueReady && !yellowReady) {
      // Fill the first 1/3 of both strips with blue (only for blue player)
      fill_solid(leds1, readyLength1, CRGB::Blue);  // First 1/3 of strip 1
      fill_solid(leds2, readyLength2, CRGB::Blue);  // First 1/3 of strip 2

      // Fill the remaining 2/3 of both strips with a moving rainbow effect
      fill_rainbow(&leds1[readyLength1], NUM_LEDS_1 - readyLength1, gHue, 7);  // Rest of strip 1
      fill_rainbow(&leds2[readyLength2], NUM_LEDS_2 - readyLength2, gHue, 7);  // Rest of strip 2
    } 
    // If yellow player is ready, show yellow on last 1/3 of both strips, rainbow on the rest
    else if (yellowReady && !blueReady) {
      // Fill the last 1/3 of both strips with yellow (only for yellow player)
      fill_solid(&leds1[NUM_LEDS_1 - readyLength1], readyLength1, CRGB::Yellow);  // Last 1/3 of strip 1
      fill_solid(&leds2[NUM_LEDS_2 - readyLength2], readyLength2, CRGB::Yellow);  // Last 1/3 of strip 2

      // Fill the remaining 2/3 of both strips with a moving rainbow effect
      fill_rainbow(&leds1[0], NUM_LEDS_1 - readyLength1, gHue, 7);  // Rest of strip 1
      fill_rainbow(&leds2[0], NUM_LEDS_2 - readyLength2, gHue, 7);  // Rest of strip 2
    }

    // Increment hue for the rainbow effect
    EVERY_N_MILLISECONDS(10) { gHue++; }

    // Show the updated LED state
    FastLED.show();
    
    // If both players are ready, transition to game mode
    if (blueReady && yellowReady) {
      // Run the transition animation
      transitionToBlack();

      // Set up for game mode
      gameMode = true;  // Enter game mode
      ledIndex = NUM_LEDS_1 / 2;  // Reset LED index to center
      blueReady = false;  // Reset readiness for the next game
      yellowReady = false;
      FastLED.clear();
      FastLED.setBrightness(255);
      FastLED.show();
    }
  } else {
    // Continue running the moving LED lights (rainbow effect)
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

    // Handle button presses for game control
    if (digitalRead(BUTTON_PIN) == LOW) {
      if (!buttonPressed) {
        buttonPressTime = millis();
        if (buttonPressTime - buttonReleaseTime >= debounce) {
          ledIndex = max(END_OFFSET, ledIndex - 1);
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
          ledIndex = min(NUM_LEDS_1 - END_OFFSET - 1, ledIndex + 1);
          button2Pressed = true;
        }
      }
    } else {
      button2ReleaseTime = millis();
      button2Pressed = false;
    }

    if (ledIndex - ledWidth / 2 <= END_OFFSET || ledIndex + ledWidth / 2 >= NUM_LEDS_1-END_OFFSET) {
      handleGameEnd();
    }
  }
}

void handleGameEnd() {
  CRGB winnerColor;
  CRGB sparkleColor;
  int startOffset, endOffset;
  int sparkleStart, sparkleEnd;

  if (ledIndex - ledWidth / 2 <= END_OFFSET) {
    winnerColor = CRGB::Blue; // Blue wins
    sparkleColor = CRGB::White; // Sparkles with white if blue wins
   startOffset = NUM_LEDS_1 - 1;
    endOffset = -1; // Pull everything to the Yellow side
    sparkleStart = 0; // Sparkle only on the yellow side
    sparkleEnd = NUM_LEDS_1 / 2 - 1;
  } else {
    winnerColor = CRGB::Yellow; // Yellow wins
    sparkleColor = CRGB::Magenta; // Sparkles with red if yellow wins
     startOffset = 0;
  endOffset = NUM_LEDS_1; // Pull everything to the Blue side
    sparkleStart = NUM_LEDS_1 / 2; // Sparkle only on the blue side
    sparkleEnd = NUM_LEDS_1 - 1;
  }

  // Fill both strips instantly with the winner color
  fill_solid(leds1, NUM_LEDS_1, winnerColor);
  fill_solid(leds2, NUM_LEDS_2, winnerColor);
  FastLED.show();
  delay(500);  // Wait for a brief moment to emphasize the winner color

  // "Pulling" animation: Rainbow colors move from the winner's side
  int rainbowLength = 15; // The length of the rainbow section
  int rainbowSpeed = 30;  // Speed of rainbow section movement

  // Start the rainbow section from the winner's side and move it across both strips
  for (int i = 0; i < NUM_LEDS_1 + rainbowLength; i++) {
    // Sync the rainbow section to move in the same direction on both strips
    int rainbowStart1 = (startOffset < endOffset) ? i : NUM_LEDS_1 - 1 - i;
    int rainbowStart2 = map(rainbowStart1, 0, NUM_LEDS_1 - 1, 0, NUM_LEDS_2 - 1) + LED_OFFSET_2;

    // Clear the previous rainbow section and keep the winner color in the remaining LEDs
    for (int k = 0; k < NUM_LEDS_1; k++) {
      if (k < rainbowStart1 || k >= rainbowStart1 + rainbowLength) {
        leds1[k] = winnerColor;
      }
    }
    for (int k = 0; k < NUM_LEDS_2; k++) {
      if (k < rainbowStart2 || k >= rainbowStart2 + rainbowLength) {
        leds2[k] = winnerColor;
      }
    }

    // Move the rainbow across both strips, keeping the rest in the winner's color
    for (int j = 0; j < rainbowLength; j++) {
      int currentLED1 = rainbowStart1 + j;
      int currentLED2 = rainbowStart2 + j;

      // Ensure we're within bounds for leds1 and leds2
      if (currentLED1 >= 0 && currentLED1 < NUM_LEDS_1) {
        leds1[currentLED1] = Wheel((colorIndex + currentLED1 * 10) % 255); // Rainbow color for leds1
      }
      if (currentLED2 >= 0 && currentLED2 < NUM_LEDS_2) {
        leds2[currentLED2] = Wheel((colorIndex + currentLED2 * 10) % 255); // Rainbow color for leds2
      }
    }

    FastLED.show();
    delay(rainbowSpeed);  // Adjust the speed of the rainbow movement
  }

  // Sparkle effect and celebratory animation after rainbow finishes crossing
  for (int j = 0; j < 75; j++) {
    int randomIndex1 = random(sparkleStart, sparkleEnd);  // Random index for sparkle on the winner's side
    int randomIndex2 = map(randomIndex1, 0, NUM_LEDS_1 - 1, 0, NUM_LEDS_2 - 1) + LED_OFFSET_2;

    // Sparkle with the appropriate color (white for blue, red for yellow)
    leds1[randomIndex1] = sparkleColor;
    if (randomIndex2 >= 0 && randomIndex2 < NUM_LEDS_2) {
      leds2[randomIndex2] = sparkleColor;
    }

    FastLED.show();
    delay(30);
    leds1[randomIndex1] = winnerColor; // Restore winner's color
    if (randomIndex2 >= 0 && randomIndex2 < NUM_LEDS_2) {
      leds2[randomIndex2] = winnerColor;
    }
  }

  // Final winner hold
  fill_solid(leds1, NUM_LEDS_1, winnerColor);
  fill_solid(leds2, NUM_LEDS_2, winnerColor);
  FastLED.show();
  delay(1500); // Hold winner's color for 1.5 seconds

  // Reset game state
  FastLED.clear();
  FastLED.show();

  ledIndex = NUM_LEDS_1 / 2;
  gameMode = false;
  blueReady = false;
  yellowReady = false;

  // Debugging
  Serial.println("Game End - Variables Reset:");
  Serial.print("blueReady: ");
  Serial.println(blueReady);
  Serial.print("yellowReady: ");
  Serial.println(yellowReady);
  Serial.print("gameMode: ");
  Serial.println(gameMode);
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
