// Copyright Rich Heslip 2026
//
// Plaits Library Copyright Emilie Gillete and Mark Washeim
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// ** Mutable Instrument Plaits for 2HPico **
// this is a subset of Plaits - 2HPico only has a trigger and Volts/Octave inputs so no external modulation of Timbre etc is possible
// its also missing the string machine model from Plaits 1.2 - not working yet in the ArduinoMI library
// you will need my slightly tweeked version of the ArduinoMI Plaits and STMLIB libraries to get this to compile and link correctly
//
// R Heslip Feb  2026
//
// **** requires overclocking Pico to 250Mhz ****
//
// ** Notes: if you get crackling audio on the 6opFM engines its because plaits is jumping between two patches. tweek the harmonics knob slightly till its stable
// the other way to fix this is to press and hold the button for 2nd UI page, then release. When you release it locks the knobs until you move them so the patch will be "locked" until you move the harmonics knob again

// top jack - Trigger input 
// middle jack - V/Octave CV input - 2HPico must be jumpered for CV in
// bottom jack - audio out

// button - click to advance to next model in bank (3 banks) 
//          - double click to advance to next bank of 8 models
//         - press and hold for 2nd UI page

// LED - Red=1st model in bank, Orange=2nd model, Yellow=3rd, Green=4rth, Aqua=5th, Blue=6th, Purple=7th, White=8th
//      no blinking = first bank of 8 models
//      1 short blink = 2nd bank of 8 models
//      2 short blinks= 3rd bank of 7 models

// page 1 parameters 
// Pot 1 - pitch +- 12 semitones
// pot 2 - Harmonics
// pot 3 - Timbre
// pot 4 - Morph

// page 2 parameters - hold button down for page 2
// Pot 1 - Octave -3 to +5 
// pot 2 - Mix of Plaits Out (fully CCW) and Aux Out (fully CW)
// pot 3 - LPG response
// pot 4 - LPG Time/Decay


#include <Arduino.h>
#include "stdio.h"
#include "pico/stdlib.h"
#include "2HPico.h"
#include <I2S.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>
// plaits dsp
#include <STMLIB.h>
#include <PLAITS.h>

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

#define MONITOR_CPU1  // define to enable 2nd core monitoring
//#define DEBUG   // comment out to remove debug code

#define SAMPLERATE 48000

Adafruit_NeoPixel LEDS(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

// stuff for blinkenLEDs
#define LEDPATTERN_ON 0xffff
#define LEDPATTERN_1BLINK 0xfffc
#define LEDPATTERN_2BLINKS 0xffee
#define LEDPATTERN_MAX 16 // number of bits in LED pattern
#define LEDBLINK 100 // off/on time
uint16_t LEDpatterns[]={LEDPATTERN_ON,LEDPATTERN_1BLINK,LEDPATTERN_2BLINKS};
uint32_t enginecolors[]={RED,ORANGE,YELLOW,GREEN,AQUA,BLUE,VIOLET,WHITE};
uint32_t LEDcolor;  // current LED color
int16_t LEDpattern=0xffff; // LED flash pattern 1=LED on 0 = LED off
int16_t LED_index;  // indexes through LEDpattern

uint32_t LEDtimer;  // timer for ledblinken

#define PT8211    // define for 2HPico to set up I2S for PT8211 DAC
I2S DAC(OUTPUT);  // 

// clickbutton library is used to detect clicks, doubleclicks, holds etc
ClickButton button1 (BUTTON1);
#define BUTTON_TIMER_MICROS 10000 // 100hz for button

#define NUMUISTATES 2
enum UIstates {SET1,SET2} ;
uint8_t UIstate=SET1;

#define NUM_BANKS 3
uint8_t engine=0;
uint8_t bank=0;
float octave=-1.0;
#define OUTPUTMAX 32000  // output gain reduction - its a bit hot on the 6opFM models
int16_t outputmix=OUTPUTMAX; // mix of normal and aux output

float trigger_in = 0.0f;

#define CVIN_VOLT 580.6  // a/d count per volt - **** adjust this value to calibrate V/octave input

plaits::Modulations modulations;
plaits::Patch patch;
plaits::Voice voice;
plaits::Voice::Frame outputPlaits[plaits::kBlockSize];

stmlib::BufferAllocator allocator;

const size_t   kBlockSize = plaits::kBlockSize;

struct Unit {
  plaits::Voice       *voice_;
  plaits::Modulations modulations;
  plaits::Patch       patch;
  char                *shared_buffer;
};

struct Unit voices[1];

// initialize Plaits voice parameters
void initPlaits() {
  voices[0].shared_buffer = (char*)malloc(32756);
  memset(voices[0].shared_buffer, 0, 32756);
  stmlib::BufferAllocator allocator(voices[0].shared_buffer, 32756);
  voices[0].voice_ = new plaits::Voice;
  voices[0].voice_->Init(&allocator);
  memset(&voices[0].patch, 0, sizeof(voices[0].patch));
  memset(&voices[0].modulations, 0, sizeof(voices[0].modulations));
  voices[0].patch = patch;  // initialize patch settings to something reasonable
  voices[0].patch.engine = 0;  
  voices[0].patch.decay = 0.5f; 
  voices[0].patch.lpg_colour = 0.5f;
  voices[0].patch.note = 60.0;
  voices[0].patch.harmonics = 0.5;
  voices[0].patch.morph = 0.5;
  voices[0].patch.timbre = 0.5;
  voices[0].modulations.trigger_patched = true; // trigger is the only plaits "modulation" input we are using here
}


// RP2040 timer code from https://github.com/raspberrypi/pico-examples/blob/master/timer/timer_lowlevel/timer_lowlevel.c
// Use alarm 0
#define ALARM_NUM 0
#define ALARM_IRQ timer_hardware_alarm_get_irq_num(timer_hw, ALARM_NUM)

static void alarm_in_us(uint32_t delay_us) {
  hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);
  irq_set_exclusive_handler(ALARM_IRQ, alarm_irq);
  irq_set_enabled(ALARM_IRQ, true);
  alarm_in_us_arm(delay_us);
}

static void alarm_in_us_arm(uint32_t delay_us) {
  uint64_t target = timer_hw->timerawl + delay_us;
  timer_hw->alarm[ALARM_NUM] = (uint32_t) target;
}

static void alarm_irq(void) {
  button1.service();    // check the button input
  hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM); // clear IRQ flag
  alarm_in_us_arm(BUTTON_TIMER_MICROS);  // reschedule interrupt
}


void setup() {
  Serial.begin(115200);

// set up I/O pins
 
#ifdef MONITOR_CPU1 // for monitoring 2nd core CPU usage
  pinMode(CPU_USE,OUTPUT); // hi = CPU busy
#endif 

  pinMode(TRIGGER,INPUT); // gate/trigger in 
  pinMode(AIN1,INPUT); // 2nd jack is CV in
  pinMode(BUTTON1,INPUT_PULLUP); // button in
  pinMode(MUXCTL,OUTPUT);  // analog switch mux

  LEDS.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  LEDS.setPixelColor(0, RED); 
  LEDcolor=RED;
  LEDS.show();

  analogReadResolution(AD_BITS); // set up for max resolution
// initialize the pot readings
  for (int16_t i=0; i<NUMPOTS;++i) {
    pot[i]=0;
    potlock[i]=0;
  }

  initPlaits();

// set up Pico I2S for PT8211 stereo DAC
	DAC.setBCLK(BCLK);
	DAC.setDATA(I2S_DATA);
	DAC.setBitsPerSample(16);
	DAC.setBuffers(1, 128, 0); // DMA buffer - 32 bit L/R words
  #ifdef PT8211
	DAC.setLSBJFormat();  // needed for PT8211 which has funny timing
  #endif
	DAC.begin(SAMPLERATE);

  alarm_in_us(BUTTON_TIMER_MICROS); // start the button IRQ
}


void loop() {

  ClickButton::Button b=button1.getButton();
    switch (b) {
    case ClickButton::Clicked:
      ++engine;    
      engine=engine&7;  // click stays in same bank
      voices[0].patch.engine=engine + bank*8; // set engine
      LEDcolor=enginecolors[engine]; // show in banks of 8
      LEDpattern=LEDpatterns[bank]; // set blink pattern for bank
      break;
    case ClickButton::DoubleClicked:  // double click through banks
      ++bank;
      if (bank >= NUM_BANKS) bank=0;
      voices[0].patch.engine=engine + bank*8; // set engine
      LEDcolor=enginecolors[engine]; // show in banks of 8
      LEDpattern=LEDpatterns[bank]; // set blink pattern for bank
      break;
    case ClickButton::Held:  // hold button to do 2nd page of parameters
      lockpots();
      UIstate=SET2;
      break;
    case ClickButton::Released:
      lockpots();
      UIstate=SET1;
      break;
    default:
      break;
  }

// set pitch from CV input
  float pitch=(float)(AD_RANGE-sampleCV2())/(float)CVIN_VOLT; // CV in is inverted
  voices[0].modulations.note=36.f +  pitch * 12.f + octave*12.f; // modulations.note is a float - its not quantized to MIDI note frequencies but appears to use MIDI note numbers

  samplepots(); // set parameters from panel pots

  switch (UIstate) {
    case SET1:
      if (!potlock[0]) voices[0].patch.note=mapf(pot[0],0,AD_RANGE-1,-12.0,12.0); // 
      if (!potlock[1]) voices[0].patch.harmonics=mapf(pot[1],0,AD_RANGE-1,0,1.0); // top pot on the panel - select DX7 patch
      if (!potlock[2]) voices[0].patch.timbre=mapf(pot[2],0,AD_RANGE-1,0,1.0);  // 
      if (!potlock[3]) voices[0].patch.morph=mapf(pot[3],0,AD_RANGE-1,0,1.0); // 
      
      break;
    case SET2:
      if (!potlock[0]) octave=mapf(pot[0],0,AD_RANGE-1,-3,4); // 8 octave range
      if (!potlock[1]) outputmix=map(pot[1],0,AD_RANGE-1,OUTPUTMAX,0); 
      if (!potlock[2]) voices[0].patch.lpg_colour=mapf(pot[2],0,AD_RANGE-1,0,1.0); 
      if (!potlock[3]) voices[0].patch.decay=mapf(pot[3],0,AD_RANGE-1,0,1.0);     
      break;
    default:
      break;
  }

  trigger_in=(float)!digitalRead(TRIGGER);
  voices[0].modulations.trigger = trigger_in;

  if ((millis()-LEDtimer) > LEDBLINK) {
    ++LED_index;
    if (LED_index > LEDPATTERN_MAX) LED_index=0;
    if (_BV(LED_index) & LEDpattern) {
      LEDS.setPixelColor(0,LEDcolor);
    }
    else LEDS.setPixelColor(0,0 ); // do off flash
    LEDS.show();  // LED will get set
    LEDtimer=millis();
  }
}



// second core dedicated to DSP

void setup1() {
  delay (200); // wait for main core to start up perhipherals
}

void loop1() {
  int32_t mix;
  voices[0].voice_->Render(voices[0].patch, voices[0].modulations,  outputPlaits, plaits::kBlockSize);

#ifdef MONITOR_CPU1  
  digitalWrite(CPU_USE,0); // low - CPU not busy
#endif

  for (size_t i = 0; i < plaits::kBlockSize; i++) {
       // write samples to DMA buffer - this is a blocking call so it stalls when buffer is full
      mix=outputPlaits[i].out*outputmix+outputPlaits[i].aux*(OUTPUTMAX-outputmix); // mix normal and aux outs
      mix=mix>>15;   // int math should be faster
      DAC.write(int16_t(mix)); // left
	    DAC.write(int16_t(mix)); // right
  }

#ifdef MONITOR_CPU1
  digitalWrite(CPU_USE,1); // hi = CPU busy
#endif
}
