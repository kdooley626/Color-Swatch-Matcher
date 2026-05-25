#include <Wire.h>
#include "Adafruit_TCS34725.h"

// Create sensor object
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_50MS,
  TCS34725_GAIN_4X
);

// Set up RBG struct
struct RGB {
  float r;
  float g;
  float b;
};

// Define functions that creates an RGB struct from the sensor reading
RGB getNormalizedRGB() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  RGB color;
  if (c==0) {
    color.r = color.g = color.b = 0;
  } else {
    color.r = (float)r / c;
    color.g = (float)g / c;
    color.b = (float)b / c;
  }
  
  return color;
}
// Define a function that creates an RGB struct from an average of a set of readings

RGB getAveragedNormalizedRGB(int samples = 5) {
  RGB sum = {0, 0, 0};
  int validSamples = 0;
  for (int i =0; i < samples; i++) {
    RGB reading = getNormalizedRGB();
    if (reading.r !=0 || reading.g != 0 || reading.b != 0) {
      sum.r += reading.r;
      sum.g += reading.g;
      sum.b += reading.b;
      validSamples++;
    }
    delay(50);
  }
  Serial.print("Sample number: "); Serial.println(validSamples);
  RGB avg = {sum.r/validSamples, sum.g/validSamples, sum.b/validSamples};
  return avg;
}


// Set up for serial input
//String inputString = "";
//bool commandComplete = false;

void setup() {
  Serial.begin(9600);

}


void loop() {
  if (Serial.available() > 0) {
    Serial.println("Recieved command");
    

    String cmd = Serial.readStringUntil('\n');
    if (cmd.equalsIgnoreCase("get sensor status")) {
      if (tcs.begin()) {
        Serial.println("TCS34725 sensor found!");
        Serial.println("DONE");
      } else {
      Serial.println("No TCS34725 found... check wiring");
      Serial.println("DONE");
      while (1);
      // put sensor in standby initially
      tcs.disable();
      }
    }
    else if (cmd.equalsIgnoreCase("get reading")) {
      

      // Wake sensor
      tcs.enable();
      delay(60); // allow integration time

      int readingNumber = 5;
      RGB avgColor = getAveragedNormalizedRGB(readingNumber);
      Serial.print("Taking average reading from "); Serial.print(readingNumber); Serial.println(" values");
      Serial.println("Average of normalized values:");
      Serial.print("R: "); Serial.print(avgColor.r, 4);
      Serial.print(" G: "); Serial.print(avgColor.g, 4);
      Serial.print(" B: "); Serial.println(avgColor.b, 4);
      Serial.print(avgColor.r, 4); Serial.print(", "); Serial.print(avgColor.g, 4); Serial.print(", "); Serial.println(avgColor.b, 4);
      // Return to standby
      tcs.disable();
      Serial.println("DONE");
    } 
    else if (cmd.equalsIgnoreCase("ping")) {
      Serial.println("PONG");
      Serial.println("DONE");
      
    }
    else {
      Serial.println("Unknown command");
      Serial.println("DONE");
    }

  } 
}

