#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define LED_PIN D5  // Pin the LED strip is connected to

#define NUM_LEDS 50  // Number of LEDs in the strip
#define BUTTON_PIN D7  // Pin the blue button is connected to
#define BUTTON2_PIN D6  // Pin the yellow button is connected to

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
int ledIndex = NUM_LEDS/2;  // Keeps track of the current LED
unsigned long buttonPressTime = 0;
unsigned long buttonReleaseTime = 0;
unsigned long debounce = 10;  // Debounce time in milliseconds
bool buttonPressed = false;
int colorIndex = 0;

unsigned long button2PressTime = 0;
unsigned long button2ReleaseTime = 0;
bool button2Pressed = false;

int i = 0;




void setup() {
  strip.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
      strip.fill((0,0,0));
      strip.setPixelColor(NUM_LEDS/2, 255,0,255);
      strip.show();
      

}


void loop() {

colorIndex++;
  for (int i = ledIndex - 5; i <= ledIndex + 5; i++) {
    strip.setPixelColor(i, Wheel(colorIndex + 20*i));
  }
  strip.show();


  
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!buttonPressed) {
      buttonPressTime = millis();
      if (buttonPressTime - buttonReleaseTime >= debounce) {
        ledIndex--;
        strip.fill((0,0,0));
        strip.show();
        buttonPressed = true;
      }
    }
  }
  else {
    buttonReleaseTime = millis();
    buttonPressed = false;
  }

  if (digitalRead(BUTTON2_PIN) == LOW) {
        if (!button2Pressed) {
      button2PressTime = millis();
      if (button2PressTime - button2ReleaseTime >= debounce) {
        ledIndex++;
        strip.fill((0,0,0));
        strip.show();
        button2Pressed = true;
      }
    }
  }
  else {
    button2ReleaseTime = millis();
    button2Pressed = false;
  }

  
if (ledIndex -5 <= 0 || ledIndex + 5 >= NUM_LEDS) {  
    
      if (ledIndex - 5 == 0) {
       // strip.fill(strip.Color(0, 0, 255)); // blue wins
       for (int i = 0; i <= NUM_LEDS; i++) {
          strip.setPixelColor(i, Wheel(85+(240/NUM_LEDS)*(-i)));
          strip.show();
          delay (75);
       }
       
       
      } else {
        //strip.fill(strip.Color(255, 255, 0)); // yellow wins
         for (int i = NUM_LEDS; i >= 0; i--) {
            strip.setPixelColor(i, Wheel(145+(240/NUM_LEDS)*i));
            strip.show();
            delay (75);
         }
      
      }
    delay (2000);  
    ledIndex = NUM_LEDS/2;
    strip.fill((0,0,0));
    strip.show();
    delay (2000);
  }

}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

     
