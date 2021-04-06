
#include "FastLED.h"
#define NUM_LEDS 7
CRGB leds[NUM_LEDS];
#define NEOPIXEL_DATA_PIN 6
#define SOUND_BUF_LEN 5
#define LOUDNESS_THRESHOLD 365
#define LOUDNESS_TIMER_COUNT 100
#define HUE_CLR_GREEN 96
#define HUE_CLR_RED 255

int sensorPin =A0 ;  // define analog port A0
int value = 0;    //set value to 0
int SOUND_BUF[SOUND_BUF_LEN];
int loudnessTimer = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //set the baud rate to 9600

  // zero out sound value buffer to start fresh
  for (int idx = 0; idx < SOUND_BUF_LEN; idx++) {
    SOUND_BUF[idx] = 0;
  }

  FastLED.addLeds<NEOPIXEL, NEOPIXEL_DATA_PIN>(leds, NUM_LEDS);
}

void loop() 
{
  int hueMeanVal;

  // retreive current audio sound level
  value = analogRead(sensorPin);  //set the value as the value read from A0
  if (value >= 300) {
    Serial.println(value, DEC);  //print the value and line wrap
  }

  // map audio value to hue color
  int hueVal = map(value, 0, LOUDNESS_THRESHOLD, HUE_CLR_GREEN, HUE_CLR_RED);
  int hueSum = hueVal;

  // if we reach a loadness threshold, start the timer
  if (value >= LOUDNESS_THRESHOLD) {
      loudnessTimer = LOUDNESS_TIMER_COUNT;
  }

  // calc sum of buffer minus the oldest value
  for (int idx = 1; idx < SOUND_BUF_LEN; idx++) {
    hueSum += SOUND_BUF[idx];
    // shift values in sound buffer queue
    SOUND_BUF[idx - 1] = SOUND_BUF[idx];
  }

  // add hueVal to end of sound buffer queue
  SOUND_BUF[SOUND_BUF_LEN - 1] = hueVal;

  // decrement the counter
  if (loudnessTimer > 0) {
    loudnessTimer--;
    hueMeanVal = 0; // set to red
  } else {
    // compute average sound level of buffer
    hueMeanVal = hueSum / SOUND_BUF_LEN;
  }

  // update colors for each LED
  for(int dot = 0; dot < NUM_LEDS; dot++) {
    leds[dot].setHSV( hueMeanVal, 255, 255);

    // clear this led for the next time around the loop
    //leds[dot] = CRGB::Black;
  }

  FastLED.show();
  delay(60);
}
