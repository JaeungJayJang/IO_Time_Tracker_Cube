// Adafruit IO Time Tracking Cube
// Tutorial Link: https://learn.adafruit.com/time-tracking-cube
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Brent Rubell for Adafruit Industries
// Copyright (c) 2019 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.
//
// Modified the original distribution to include few more features
// As the featherWing provide tap detection, I include a tap to stop timer.
//
// Modified by Jaeung Jang.
/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

/************************ Example Starts Here *******************************/
#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>

// Prop-Maker Wing
#define NEOPIXEL_PIN 2
#define POWER_PIN 15

// Used for Pizeo
// #define PIEZO_PIN 0

// # of Pixels Attached
#define NUM_PIXELS 8

// Adafruit_LIS3DH Setup
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// Adjust this number for the sensitivity of the 'click' force
// this strongly depend on the range! for 16G, try 5-10
// for 8G, try 10-20. for 4G try 20-40. for 2G try 40-80
#define CLICKTHRESHHOLD 20

// NeoPixel Setup
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGBW + NEO_KHZ800);

// Set up the 'cubeTask' feed
AdafruitIO_Feed* cubetask = io.feed("TimeTracker");

/* Time Tracking Cube States
 * 1: Cube Tilted Left
 * 2: Cube Tilted Right
 * 3: Cube Tilted Away
 * 4: Cube Tilted Toward
 * 5: Cube Neutral, Top
 * 6: Cube Flipped
*/
int cubeState = 5;
int prvCubeState = 0;

// Keeping touch state
bool touched = false;

// keeping color
// uint32_t color = 0;
uint32_t r = 0;
uint32_t g = 0;
uint32_t b = 0;
uint32_t w = 0;

float angle = 0;

// Tasks (change these to what you're currently working on)
const char* taskBase = "Base State";
const char* taskOne = "Coffee Time";
const char* taskTwo = "Programming";
const char* taskThree = "Relaxation";
;
// const char* taskFour = "Write Code";
// const char* taskFive = "Write Code";
// const char* taskSix = "Write Code";
String currentTask = taskBase;

// Adafruit IO sending delay, in seconds
int sendDelay = 0.5;

// Time-Keeping
unsigned long currentTime;
unsigned long prevTime;
unsigned long breakTime = 0;
unsigned long initialEventTime = 0;
int seconds = 0;
int minutes = 0;

// -------------------------------------------------------------------------------------------

void updatePixels(uint8_t red, uint8_t green, uint8_t blue, uint8_t white) {
  // strip.setPixelColor(0, 50, 0, 0);
  // strip.show();
  strip.fill(strip.Color(green, red, blue, white));
  strip.show();
}

void setTaskColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t white, String task) {
  r = red;
  g = green;
  b = blue;
  w = white;
  if (task != taskBase){
    cubetask->save("Start: " + task);
  }
  currentTask = task;
  updatePixels(r, g, b, w);
}


void updateTime() {
  // grab the current time from millis()
  currentTime = millis() / 1000;
  seconds = currentTime - prevTime;
  // increase mins.
  if (seconds == 60) {
    prevTime = currentTime;
    minutes++;
  }
}

int check_the_face(sensors_event_t event) {
  // State mapping:
  // 1: Left, 2: Right, 3: Front, 4: Back, 5: Up, 6: Down
  if (event.acceleration.x > 8 && event.acceleration.x < 10) {
    return 1;  // Left
  } else if (event.acceleration.x < -8 && event.acceleration.x > -11) {
    return 2;  // Right
  } else if (event.acceleration.y > 8 && event.acceleration.y < 10) {
    return 3;  // Front
  } else if (event.acceleration.y < -8 && event.acceleration.y > -11) {
    return 4;  // Back
  } else if (event.acceleration.z > 8 && event.acceleration.z < 10) {
    return 5;  // Up
  } else if (event.acceleration.z < -8 && event.acceleration.z > -11) {
    return 6;  // Down
  } else {
    return 0;  // Neutral/Base state
  }
}

void blink(uint16_t duration = 300) {
  updatePixels(0, 0, 0, 0);
  delay(duration);
  updatePixels(r, g, b, w);
  delay(duration);
}

// --------  set up  -----------------------------------------------------------------------------------

void setup() {
  // start the serial connection
  Serial.begin(9600);
  // wait for serial monitor to open
  Serial.println("Adafruit IO Time Tracking Cube");

  // disabling low-power mode on the prop-maker wing
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);


  // Initialize LIS3DH
  if (!lis.begin(0x18)) {
    Serial.println("Couldnt start");
    while (1)
      ;
  }
  Serial.println("LIS3DH found!");
  lis.setRange(LIS3DH_RANGE_16_G);

  // 0 = turn off click detection & interrupt
  // 1 = single click only interrupt output
  // 2 = double click only interrupt output, detect single click
  // Adjust threshhold, higher numbers are less sensitive
  lis.setClick(2, CLICKTHRESHHOLD);

  // Initialize NeoPixel Strip
  strip.begin();
  Serial.println("Pixels init'd");

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
}

// --------  loop  -----------------------------------------------------------------------------------

void loop() {
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  // Update the timer
  updateTime();

  // Get a normalized sensor reading
  sensors_event_t event;
  lis.getEvent(&event);

  // check the cube state over 0.1 sec
  // to fully sure that it's the face; eliminate false state call
  int faceState = check_the_face(event);
  if (faceState == 0) return;

  delay(300);  // simple debounce
  // If the cube's face changes, handle the state update immediately
  if (check_the_face(event) == faceState) {
    prvCubeState = cubeState;
    cubeState = faceState;

    // If the face has changed, update the task and Neopixels
    if (cubeState != prvCubeState) {
      if (touched) {
        touched = false; // Reset touched state on face change
        if (currentTask != taskBase) {
          cubetask->save("Resume: " + currentTask);
        }
      }

      if (currentTask != taskBase) {
        cubetask->save("Stop: " + currentTask);
      }

      // Update the task and Neopixels based on the cube's new orientation
      switch (cubeState) {
        case 1: setTaskColor(150, 75, 0, 50, taskOne); break; // Task 1 (left)
        case 2: setTaskColor(0, 128, 255, 30, taskTwo); break; // Task 2 (right)
        case 3: setTaskColor(255, 165, 0, 30, taskThree); break; // Task 3 (front)
        case 4: setTaskColor(200, 200, 0, 0, taskBase); break;  // Base task (back)
        case 5: setTaskColor(0, 0, 0, 0, taskBase); break;      // Base task (up)
        case 6: setTaskColor(0, 200, 200, 0, taskBase); break;  // Base task (down)
      }

      delay(500); // Optional delay for smoother transitions
      return;  // Exit loop after handling orientation change (high priority)
    }
  }

  // Now check for tap events (secondary)
  uint8_t click = lis.getClick();
  if (click & 0x1 || click & 2 || click & 4) {
    // Handle tap event only if no face change occurred
    touched = !touched;
    if (currentTask != taskBase) {
      if (touched) {
        cubetask->save("Break: " + currentTask);
      } else {
        cubetask->save("Resume: " + currentTask);
      }
    }

    // Visual feedback for tap
    delay(400);
  }

  // Handle blinking when touched
  if (touched) {
    blink();
  } else {
    updatePixels(r, g, b, w);  // Keep current pixel color when not touched
  }
}
