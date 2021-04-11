
#include "FastLED.h"
#define NUM_LEDS 7
CRGB leds[NUM_LEDS];
#define NEOPIXEL_DATA_PIN 6
#define MODE_TOGGLE_SWITCH_PIN 7
#define POT_PIN A6
#define AUDIO_SENSOR_PIN A3
#define SOUND_BUF_LEN 5
#define LOUDNESS_THRESHOLD 365
#define LOUDNESS_TIMER_COUNT 100
#define HUE_CLR_GREEN 96
#define HUE_CLR_RED 255

int audioValue = 0;
int SOUND_BUF[SOUND_BUF_LEN];
int loudnessTimer = 0;
int prevPotVal = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //set the baud rate to 9600

  // set internal pull up resistor for toggle switch pin
  pinMode(MODE_TOGGLE_SWITCH_PIN, INPUT_PULLUP);

  // zero out sound value buffer to start fresh
  for (int idx = 0; idx < SOUND_BUF_LEN; idx++) {
    SOUND_BUF[idx] = 0;
  }

  FastLED.addLeds<NEOPIXEL, NEOPIXEL_DATA_PIN>(leds, NUM_LEDS);
}

void loop() 
{
  int hueMeanVal;
  int modeToggleState;
  int potVal;
  int potDiff;

  potVal = map(analogRead(POT_PIN), 0, 1023, 0, 255);
  potDiff = potVal - prevPotVal;

  if (abs(potDiff) < 10 && potVal > 0 && potVal <= 255) {
    // change hasn't deviated enough, restore original val
    potVal = prevPotVal;
  } else {
    // save new current potVal
    prevPotVal = potVal;
  }

  // retrieve current state of toggle
  modeToggleState = digitalRead(MODE_TOGGLE_SWITCH_PIN);

  // retreive current audio sound level
  audioValue = analogRead(AUDIO_SENSOR_PIN);  //set the audio value
  if (audioValue >= 300) {
    Serial.println(audioValue, DEC);  //print the value and line wrap
  }

  if (modeToggleState == HIGH) {
    // map audio value to hue color
    int hueVal = map(audioValue, 0, LOUDNESS_THRESHOLD, HUE_CLR_GREEN, HUE_CLR_RED);
    int hueSum = hueVal;

    // if we reach a loadness threshold, start the timer
    if (audioValue >= LOUDNESS_THRESHOLD) {
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
      leds[dot].setHSV( hueMeanVal, 255, potVal);

      // clear this led for the next time around the loop
      //leds[dot] = CRGB::Black;
    }
  } else {
    int brightnessVal = 255;
    for(int dot = 0; dot < NUM_LEDS; dot++) {
      // clear this led for the next time around the loop
      leds[dot].setHSV(0, 0, potVal);
    }
  }

  FastLED.show();
  delay(60);
}
