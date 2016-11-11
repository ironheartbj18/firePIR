#include "FastLED.h"
#define NUM_LEDS 60 
CRGB leds[NUM_LEDS];
#define PIN 6 
int calibrationTime = 10; 
unsigned long timer;
unsigned long interval = 5000;  
unsigned long previousMillis = 0;

int pirPin = 11;    //the digital pin connected to the PIR sensor's output
int ledPin = 13;    // the dignial pin connected to the led 
int LDRSensor = A0; // Light dependant resistor
int LDRValue = 0;   // Variable to hold the LDR value

void setup(){
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  digitalWrite(pirPin, LOW);
  FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  delay(2000); // it takes the sensor 2 seconds to scan the area around it before it can 
  //give the sensor some time to calibrate
  Serial.print("calibrating sensor please wait for 10 secs ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(50);
}

void loop() {
  LDRValue = analogRead(LDRSensor);
  Serial.println(LDRValue);
  
  previousMillis = millis();
  if(LDRValue > 900){
    int pir1 = digitalRead(pirPin);  // Constantly poll the PIR
    if(pir1 == HIGH ){
      Fire(55,120,15); //  it's working
      FastLED.show();
      digitalWrite(ledPin, LOW); 
      Serial.println("SAW YOU!"); 
      timer = millis();  // Timestamp when the PIR is triggered.  The LED cycle wil then start.
      }
       
    if (timer+10000 < millis()) {  
      FastLED.clear(true);
      digitalWrite(ledPin, LOW); // lights off
      Serial.println("THE END");
      }
   }
    else{
      FastLED.clear(true);
      digitalWrite(ledPin, HIGH); // lights on
    }
}

void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;
  
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
    
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
  
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
    
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}
// *** REPLACE TO HERE ***

void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H 
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}
