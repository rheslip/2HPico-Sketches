// Copyright 2026 Rich Heslip
//
// Author: Rich Heslip 
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
/*
 Mutable Instruments Braids for 2HPico March 2026

 Braids using the Pico-Audio framework which is a port of PJRC's Teensy Audio framework
 the Braids code was ported to the Teensy Audio framework by the MicroDexed project - at least thats where I found it
 runs OK at 150 Mhz - CPU utilization is quite low

Button - press to move between parameter pages

Top Jack - Gate/Trigger input 

Middle jack - CV input - solder the CV jumper on the back of the main PCB

Bottom Jack - Audio output

First Parameter Page - RED

Top pot - Braids model - there are 43 models so this can be tricky to dial in. once you have it press the button which will lock that setting in until you change it

Second pot - Timbre

Third pot - Color

Fourth pot - Frequency


Second Parameter Page -  GREEN

Top pot - Attack

Second pot - Decay

Third pot - Sustain

Fourth pot - Release

*/

#include "2HPico.h"
#include "synth_braids.h"
#include <Adafruit_NeoPixel.h>
#include <math.h>
#include "pico/multicore.h"
#include <pico-audio.h>

#define DEBUG   // comment out to remove debug code

Adafruit_NeoPixel LEDS(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

#define NUMUISTATES 2
enum UIstates {SET1,SET2} ;
uint8_t UIstate=SET1;

bool trigger=0;
bool button=0;
int16_t shape;

#define DEBOUNCE 10
uint32_t buttontimer,trigtimer,parameterupdate;

#define CVIN_VOLT 580.6  // a/d count per volt - **** adjust this value to calibrate V/octave input
int16_t minfreq=10;

// Pico Audio framework patch setup
// sample rate is fixed at 48k in the Pico Audio library
// you can change it by editing the definitions in AudioStream.h

AudioSynthBraids         br;
AudioOutputPT8211          i2s1;
AudioEffectEnvelope     adsr;
AudioConnection          patchCord0(br, adsr);
AudioConnection          patchCord1(adsr, 0, i2s1, 0);
AudioConnection          patchCord2(adsr, 0, i2s1, 1);

void setup() {

  Serial.begin(115200);

  pinMode(TRIGGER,INPUT_PULLUP); // gate/trigger in, not used here
  pinMode(BUTTON1,INPUT_PULLUP); // button in
  pinMode(MUXCTL,OUTPUT);  // analog switch mux

  LEDS.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  LEDS.setPixelColor(0, RED); 
  LEDS.show();

  analogReadResolution(AD_BITS); // set up for max resolution

  // give the audio library some memory.  We'll be able
  // to see how much it actually uses, which can be used
  // to reduce this to the minimum necessary.
  AudioMemory(10);  // 

	i2s1.begin(BCLK,WS,I2S_DATA); // set up I2S for PT8211
}


int speed = 60;

// first core handles Pico Audio processing under interrupts
// print the audio stats in the foreground
void loop() {

  // print a summary of the current & maximum usage

  Serial.printf("%d %s ",shape,br.get_name(shape));
  Serial.print("all=");
  Serial.print(AudioProcessorUsage());
  Serial.print(",");
  Serial.print(AudioProcessorUsageMax());
  Serial.print("    ");
  Serial.print("Memory: ");
  Serial.print(AudioMemoryUsage());
  Serial.print(",");
  Serial.print(AudioMemoryUsageMax());
  Serial.print("    ");

  Serial.println();
  delay(speed);

}

// second core setup
// second core is dedicated to UI
void setup1() {
delay (1000); // wait for main core to start up peripherals
}

void loop1() {

  if (!digitalRead(BUTTON1)) {
    if (((millis()-buttontimer) > DEBOUNCE) && !button) {  // if button pressed advance to next parameter set
      button=1;  
      ++UIstate;
      if (UIstate >= NUMUISTATES) UIstate=SET1;
      lockpots();
    }
  }
  else {
    buttontimer=millis();
    button=0;
  }

  if ((millis() -parameterupdate) > PARAMETERUPDATE) {  // don't update the parameters too often 
    parameterupdate=millis();
    samplepots();

// set synth parameters from panel pots
// 
    switch (UIstate) {
        case SET1:
          LEDS.setPixelColor(0, RED); 
          if (!potlock[0]) {
            shape=(map(pot[0],0,AD_RANGE-1,0,41)); //
            br.set_braids_shape(shape); //
          }
          if (!potlock[1]) br.set_braids_timbre(map(pot[1],0,AD_RANGE-1,0,32767)); //
          if (!potlock[2]) br.set_braids_color(map(pot[2],0,AD_RANGE-1,0,32767)); //
          if (!potlock[3]) minfreq=(map(pot[3],0,AD_RANGE-1,10,60*128)); // see note on pitch below
          break;
        
        case SET2:
          LEDS.setPixelColor(0, GREEN);             
          if (!potlock[0]) adsr.attack(pow(mapf(pot[0],0,AD_RANGE-1,0,21),3));  // time is in milliseconds, exponential response
          if (!potlock[1]) adsr.decay(pow(mapf(pot[1],0,AD_RANGE-1,0,21),3));  
          if (!potlock[2]) adsr.sustain(mapf(pot[2],0,AD_RANGE-1,0,1)); //      
          if (!potlock[3]) adsr.release(pow(mapf(pot[3],0,AD_RANGE-1,0,21),3)); 
          break;
        default:
          break;
    }
  }
 
  if (!digitalRead(TRIGGER)) {
    if (((millis()-trigtimer) > TRIG_DEBOUNCE) && !trigger) {  // trigger detection
      trigger=1; 
      float cv=(AD_RANGE-sampleCV2()); // CV in is inverted 
    // pitch seems to be in MIDI notes with a 7 bit fractional part
      br.set_braids_pitch((int16_t)(cv/CVIN_VOLT*12*128)+minfreq); // ~ 7 octave range
      adsr.noteOn(); // trigger the ADSR 
    }
  }
  else {
    trigtimer=millis();
    if (trigger) adsr.noteOff();  // gate/trigger just went low
    trigger=0;   
  }
  LEDS.show();  // update LED
}
