/* Copyright Rich Heslip 2026
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

// ** Mutable Instrument Braidss for 2HPico **

//
// R Heslip March  2026
//
//
** Notes: if you get crackling audio its because braids is jumping between two patches. tweek the model knob slightly till its stable
the other way to fix this is to press the button. This switches parameter pages and locks the knob settings. The patch will be "locked" until you move the model knob again.

top jack - Trigger input 
middle jack - V/Octave CV input - 2HPico must be jumpered for CV in
bottom jack - audio out

button - click to advance to next page

page 1 parameters - RED
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

#include <Arduino.h>
#include "stdio.h"
#include "pico/stdlib.h"
#include "2HPico.h"
#include <I2S.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>
// plaits dsp
#include <STMLIB.h>
#include <BRAIDS.h>

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

#define MONITOR_CPU1  // define to enable 2nd core monitoring
//#define DEBUG   // comment out to remove debug code


#define SAMPLERATE 48000
#define GATE TRIGGER    // semantics - ADSR is generally used with a gate signal

Adafruit_NeoPixel LEDS(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

uint32_t buttontimer,parameterupdate,gatetimer;  // timers

bool button,gate;
int16_t shape=0;
int16_t timbre=0;
int16_t color=0;

#define GATE_DELAY 5  // gate debounce time + delay to allow CV to settle

#define PT8211    // define for 2HPico to set up I2S for PT8211 DAC
I2S DAC(OUTPUT);  // 

#include "daisysp.h"
// including the source files is a pain but that way you compile in only the modules you need
// DaisySP statically allocates memory and some modules e.g. reverb use a lot of ram
#include "control/adsr.cpp"
Adsr      env;

#define NUMUISTATES 2
enum UIstates {SET1,SET2} ;
uint8_t UIstate=SET1;

#define CVIN_VOLT 580.6  // a/d count per volt - **** adjust this value to calibrate V/octave input
int16_t minfreq=10;

// braids library stuff
#define     BLOCK_SIZE          32      // --> macro_oscillator.h !

struct Unit {
  braids::MacroOscillator *osc;
  braids::Quantizer   *quantizer;
  braids::SignatureWaveshaper *ws;
  int16_t     buffer[BLOCK_SIZE];
  uint8_t     sync_buffer[BLOCK_SIZE];
} voices[1];

// initialize voice parameters
void initVoices() {
  voices[0].osc = new braids::MacroOscillator;
  voices[0].osc->Init(SAMPLERATE);
  voices[0].osc->set_pitch((48 << 7));
  voices[0].osc->set_shape((braids::MacroOscillatorShape)0);
  voices[0].ws = new braids::SignatureWaveshaper;
  voices[0].ws->Init(123774);   // RH where did this magic number come from?
  voices[0].quantizer = new braids::Quantizer;
  voices[0].quantizer->Init();
  voices[0].quantizer->Configure(braids::scales[0]);
  memset(voices[0].buffer, 0, sizeof(int16_t)*BLOCK_SIZE);
  memset(voices[0].sync_buffer, 0, sizeof(voices[0].sync_buffer));
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
  LEDS.show();

  analogReadResolution(AD_BITS); // set up for max resolution
// initialize the pot readings
  for (int16_t i=0; i<NUMPOTS;++i) {
    pot[i]=0;
    potlock[i]=0;
  }

// init the DaisySP ADSR
  env.Init(SAMPLERATE);
  env.SetTime(ADENV_SEG_DECAY, 0.4f);

  // init the braids voices
  initVoices();

// set up Pico I2S for PT8211 stereo DAC
	DAC.setBCLK(BCLK);
	DAC.setDATA(I2S_DATA);
	DAC.setBitsPerSample(16);
	DAC.setBuffers(1, 128, 0); // DMA buffer - 32 bit L/R words
  #ifdef PT8211
	DAC.setLSBJFormat();  // needed for PT8211 which has funny timing
  #endif
	DAC.begin(SAMPLERATE);
}


void loop() {
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
            shape=(map(pot[0],0,AD_RANGE-1,0,braids::MACRO_OSC_SHAPE_LAST-1)); //
            voices[0].osc->set_shape((braids::MacroOscillatorShape)shape);
          }
          if (!potlock[1]) {
            timbre=(map(pot[1],0,AD_RANGE-1,0,32767)); //
            voices[0].osc->set_parameters(timbre, color);
          }
          if (!potlock[2]) {
            color=(map(pot[2],0,AD_RANGE-1,0,32767)); //
            voices[0].osc->set_parameters(timbre, color);
          }
          if (!potlock[3]) minfreq=(map(pot[3],0,AD_RANGE-1,10,60*128)); // see note on pitch below
          break;
        
        case SET2:
          LEDS.setPixelColor(0, GREEN);             
          if (!potlock[0]) env.SetTime(ADSR_SEG_ATTACK, pow(mapf(pot[0],0,AD_RANGE-1,0,2.1),3)); // up to 10 seconds per segment, exponential pot response
          if (!potlock[1]) env.SetTime(ADSR_SEG_DECAY, pow(mapf(pot[1],0,AD_RANGE-1,0,2.1),3));
          if (!potlock[2]) env.SetSustainLevel(mapf(pot[2],0,AD_RANGE-1,0,1));
          if (!potlock[3]) env.SetTime(ADSR_SEG_RELEASE, pow(mapf(pot[3],0,AD_RANGE-1,0,2.1),3));   
          break;
        default:
          break;
    }
  }



  if (!digitalRead(GATE)) {  // if gate input is active, tell core 1 to process ADSR
    if (((millis()-gatetimer) > GATE_DELAY) && !gate) {  
      gate=1;  
      float cv=(AD_RANGE-sampleCV2()); // CV in is inverted 
      // pitch seems to be in MIDI notes with a 7 bit fractional part
      voices[0].osc->set_pitch((int16_t)(cv/CVIN_VOLT*12*128)+minfreq); // ~ 7 octave range
      voices[0].osc->Strike();
    }
  }
  else {
    gatetimer=millis();
    gate=0;   
  }
  LEDS.show();  // update LED
}



// second core dedicated to DSP

void setup1() {
  delay (200); // wait for main core to start up perhipherals
}

void loop1() {

  float envelope;

  voices[0].osc->Render(voices[0].sync_buffer, voices[0].buffer, BLOCK_SIZE);

#ifdef MONITOR_CPU1  
  digitalWrite(CPU_USE,0); // low - CPU not busy
#endif

  for (size_t i = 0; i < BLOCK_SIZE; i++) {
       // write samples to DMA buffer - this is a blocking call so it stalls when buffer is full
      envelope=env.Process(gate);
      DAC.write((int16_t)(voices[0].buffer[i]*envelope)); // left
	    DAC.write((int16_t)(voices[0].buffer[i]*envelope)); // right
  }

#ifdef MONITOR_CPU1
  digitalWrite(CPU_USE,1); // hi = CPU busy
#endif
}
