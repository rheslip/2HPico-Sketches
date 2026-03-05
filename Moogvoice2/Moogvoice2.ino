
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
R Heslip for 2HPico March 2026

// simplified Moog voice app - 2 oscillators, Moog style lowpass filter, AD env gen
// runs OK at 150 Mhz

Top Jack - gate input 

Middle jack - Volt/Octave input

Bottom Jack - output

First Parameter Page - RED

Top pot - 1st Oscillator tuning

Second pot - 2nd Oscillator tuning

Third pot - Oscillator Waveform

Fourth pot - Filter Frequency


Second Parameter Page -  GREEN

Top pot - Filter Resonance

Second pot - Envelope to Filter Frequency mod depth 

Third pot - ADSR Attack

Fourth pot - ADSR Release 

*/

#include "2HPico.h"
#include <I2S.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>

#include "pico/multicore.h"

#define DEBUG   // comment out to remove debug code
#define MONITOR_CPU1  // define to enable 2nd core monitoring

#define GATE TRIGGER    // semantics - ADSR is generally used with a gate signal

//#define SAMPLERATE 11025 
//#define SAMPLERATE 22050  // 2HP board antialiasing filters are set up for 22khz
//#define SAMPLERATE 44100
#define SAMPLERATE 96000

Adafruit_NeoPixel LEDS(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

I2S DAC(OUTPUT);  // using PCM1103 stereo DAC

#include "daisysp.h"

// including the source files is a pain but that way you compile in only the modules you need
// DaisySP statically allocates memory and some modules e.g. reverb use a lot of ram
#include "synthesis/oscillator.cpp"
#include "control/adsr.cpp"
#include "filters/Moogladder.cpp"

float samplerate=SAMPLERATE;  // for DaisySP

#define VOICES 1
#define OSCSPERVOICE 2   // 

// parameters we can modify via MIDI CCs
int waveform=0;
float minfreq[OSCSPERVOICE] ={50,100};
float filterfreq=100;
float filtersweep=SAMPLERATE/4;
float envelopefiltermod=0.2;
float filterresonance=0.1;

// create daisySP processing objects
Oscillator osc[VOICES * OSCSPERVOICE];
Adsr      env;
MoogLadder filt;


// a/d values from pots
// pots are used for two or more parameters so we don't change the values till
// there is a significant movement of the pots when the pots are "locked"
// this prevents a waveform or level change ("shift" parameters) from changing the ramp times when the shift button is released

#define CV_VOLT 580.6  // a/d counts per volt - trim for V/octave

#define OSC_MIN_FREQ 10
#define FILTER_MIN_FREQ 10

bool gate=0;
bool button=0;

#define NUMUISTATES 2
enum UIstates {SET1,SET2} ;
uint8_t UIstate=SET1;

#define DEBOUNCE 10
uint32_t buttontimer,gatetimer,parameterupdate;

void setup() { 
  Serial.begin(115200);

#ifdef DEBUG

  Serial.println("starting setup");  
#endif

// set up I/O pins
 
#ifdef MONITOR_CPU1 // for monitoring 2nd core CPU usage
  pinMode(CPU_USE,OUTPUT); // hi = CPU busy
#endif 

  pinMode(GATE,INPUT_PULLUP); // gate/trigger in
  pinMode(BUTTON1,INPUT_PULLUP); // button in
  pinMode(MUXCTL,OUTPUT);  // analog switch mux

  LEDS.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  LEDS.setPixelColor(0, RED); 
  LEDS.show();

  for (int j=0; j< OSCSPERVOICE; ++j) {
    osc[j].Init(samplerate);       // initialize the voice objects
    osc[j].SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);  //    
    osc[j].SetFreq(minfreq[j]);
  }

  env.Init(samplerate);
  env.SetTime(ADENV_SEG_DECAY, 0.0f); // make ADSR into an AD envelope
  env.SetSustainLevel(1);
  filt.Init(samplerate);
  filt.SetRes(filterresonance); // filter resonance

  analogReadResolution(AD_BITS); // set up for max resolution

// set up Pico I2S for PT8211 stereo DAC
	DAC.setBCLK(BCLK);
	DAC.setDATA(I2S_DATA);
	DAC.setBitsPerSample(16);
	DAC.setBuffers(1, 128, 0); // DMA buffer - 32 bit L/R words
	DAC.setLSBJFormat();  // needed for PT8211 which has funny timing
	DAC.begin(SAMPLERATE);

#ifdef DEBUG  
  Serial.println("finished setup");  
#endif
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

  if ((millis() -parameterupdate) > PARAMETERUPDATE) {  // don't update the parameters too often -sometimes it messes up the daisySP models
    parameterupdate=millis();
    samplepots();

// set synth parameters from panel pots
// 
    switch (UIstate) {
        case SET1:
          LEDS.setPixelColor(0, RED); 

          if (!potlock[0]) minfreq[0]=(mapf(pot[0],0,AD_RANGE-1,20,160)); // 4 octave range
          if (!potlock[1]) minfreq[1]=(mapf(pot[1],0,AD_RANGE-1,20,160)); // 4 octave range
          
          if (!potlock[2]) {
            int8_t waveform=(map(pot[2],0,AD_RANGE-1,Oscillator::WAVE_TRI,Oscillator::WAVE_POLYBLEP_TRI)); // some waveforms mess up the oscillator - either running out of CPU or should be changing it on core 1
            for (int16_t i=0;i< OSCSPERVOICE;++i) osc[i].SetWaveform(waveform);
          }
          if (!potlock[3]) filterfreq=(mapf(pot[3],0,AD_RANGE-1,20,2500)); // 
          break;
        
        case SET2:
          LEDS.setPixelColor(0, GREEN);             
          if (!potlock[0]) filt.SetRes(mapf(pot[0],0,AD_RANGE-1,0,0.95));  // don't take resonance too high or filter behaves badly
          if (!potlock[1]) envelopefiltermod=(mapf(pot[1],0,AD_RANGE-1,0,1.0));  
          if (!potlock[2]) env.SetTime(ADSR_SEG_ATTACK, mapf(pot[2],0,AD_RANGE-1,0,2)); // up to 2 seconds per segment      
          if (!potlock[3]) env.SetTime(ADSR_SEG_RELEASE, mapf(pot[3],0,AD_RANGE-1,0,2)); 
          break;
        default:
          break;

    }
  }

  float cv=(AD_RANGE-sampleCV2()); // CV in is inverted. this number will always be at least 1 so we don't divide by zero below

  osc[0].SetFreq(pow(2,(cv/CV_VOLT))*minfreq[0]); // ~ 7 octave range
  osc[1].SetFreq(pow(2,(cv/CV_VOLT))*minfreq[1]);

  if (!digitalRead(GATE)) {  // if gate input is active, tell core 1 to process ADSR
    if (((millis()-gatetimer) > GATE_DEBOUNCE) && !gate) {  
      gate=1;  
    }
  }
  else {
    gatetimer=millis();
    gate=0;
  }

  LEDS.show();  // update LED
}


// second core setup
// second core is dedicated to sample processing
void setup1() {
delay (1000); // wait for main core to start up peripherals
}

// process audio samples
void loop1(){

static  float freq,out,sig,filtsig,envelope,outsig,wetvl, wetvr,lfomod;
static  int32_t outsample;

  sig=0;
  for (int j=0; j < OSCSPERVOICE; ++j) {
    sig+=osc[j].Process(); // sum oscillators in each voice
  }
//  sig=sig/OSCSPERVOICE; // scale down by number of oscillators
  sig=sig/2;
  envelope=env.Process(gate);

  filt.SetFreq(constrain(filterfreq+envelope*filtersweep*envelopefiltermod,1,SAMPLERATE/2));
//  filt.SetFreq(200+envelope*3000*(lfo.Process()+1));
  sig=filt.Process(sig);

  sig=sig*envelope;   // VCA
  out=sig;

  outsample = (int32_t)(out*MULT_16)>>16; // scaling 

#ifdef MONITOR_CPU1  
  digitalWrite(CPU_USE,0); // low - CPU not busy
#endif
 // write samples to DMA buffer - this is a blocking call so it stalls when buffer is full
	DAC.write(int16_t(outsample)); // left
	DAC.write(int16_t(outsample)); // right

#ifdef MONITOR_CPU1
  digitalWrite(CPU_USE,1); // hi = CPU busy
#endif
}





