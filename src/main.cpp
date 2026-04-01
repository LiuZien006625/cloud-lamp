#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 60        // Number of LEDs in your strip
#define DATA_PIN 16         // Data pin connected to WS2812B
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS 200     // Set overall brightness (0-255)

CRGB leds[NUM_LEDS];

// touch code
const int touchPin = 4; 
const int LED1 = 16;
const int threshold = 20;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 1000;
int touchValue;
int lampState = LOW;
int buttonState;
static uint8_t Shue = 0;
int lampMode = 1; // 1: thunderstorm, 2: breathing, 3: rainbow, 4: meteor


void breathing() {

  uint8_t bri = beatsin8(15, 0, 255); 
  fill_solid(leds, NUM_LEDS, CHSV(32, 200, bri)); 
  FastLED.show();
}

void rainbowFlow(uint8_t hue) {
  
  fill_rainbow(leds, NUM_LEDS, hue, 7);
  Shue++;
  fadeToBlackBy(leds, NUM_LEDS, 50);
  FastLED.show();
}

void meteor() {
  static int pos = 0;
  fadeToBlackBy(leds, NUM_LEDS, 60);
  leds[pos] = CRGB::Cyan; 
  pos++;
  if (pos >= NUM_LEDS) pos = 0;
  FastLED.show();
  delay(30); 
}

void thunderstorm() {
  // Randomly decide to create a flash
  if (random(100) < 10) { // 10% chance per cycle to start a flash

    // Randomize number of flashes
    int flashes = random(2, 5); 
    
    for (int i = 0; i < flashes; i++) {
      // Randomize which part of the strip lights up
      int startLed = random(0, NUM_LEDS / 2);
      int endLed = random(startLed, NUM_LEDS);
      
      // Choose white or slight blue color
      CRGB color = (random(0, 5) > 3) ? CRGB(100, 100, 255) : CRGB::White;
      
      // Light up the segment
      fill_solid(&leds[startLed], endLed - startLed, color);
      FastLED.show();
      
      delay(random(10, 50)); // Fast flash duration
      
      // Turn off
      fill_solid(&leds[startLed], endLed - startLed, CRGB::Black);
      FastLED.show();
      
      delay(random(10, 100)); // Dark time between flashes
    }
  }
  // Keep base scene very dark blue or off
  fadeToBlackBy(leds, NUM_LEDS, 50);
  FastLED.show();

}

void updateLEDs(int touchValue){
  Serial.print("Value: ");
  Serial.println(touchValue);
  if (touchValue >= 100 && touchValue <= 180){

    if (lampState == LOW){
      lampState = HIGH;
    } else {
      lampState = LOW;
    }
  
    Serial.print(" - lampState: ");
    Serial.println(lampState);
    lastDebounceTime = millis();

  }
}

bool debounceReady() {
  if ((millis() - lastDebounceTime) > debounceDelay) {
    return true;
  } else {
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000); // give me time to bring up serial monitor
  // initialize the LED pin as an output:
  pinMode (LED1, OUTPUT);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  touchValue = touchRead(touchPin);
  if ( debounceReady()){
  updateLEDs(touchValue);
  }

  
  if (lampState == HIGH){

    switch (lampMode) {
            case 1:
                thunderstorm();
                break;
            case 2:
                breathing();
                break;
            case 3:
                rainbowFlow(Shue);
                break;
            case 4:
                meteor();
                break;
    }
    //Randomly decide to create a flash
    // if (random(100) < 10) { // 10% chance per cycle to start a flash
    //   thunderstorm();
    //   delay(100);
    // breathing();
    //   delay(500);
      //rainbowFlow(Shue);
    //   delay(500);
    //   meteor();
    // }

    // //Keep base scene very dark blue or off
    // fadeToBlackBy(leds, NUM_LEDS, 50);
    // FastLED.show();
    }else {
      fadeToBlackBy(leds, NUM_LEDS, 50);
      FastLED.show();
    }
 delay(50);
}

