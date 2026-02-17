// Copyright 2025 Rich Heslip
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
// misc defines and support routines for the 2HPico module
// R Heslip Dec 2025

#include "2HPico_io.h"
#include "ClickButton.h"
#include "scales.h"

// for Adafruit Neopixels
#define NUMPIXELS 1 // 
#define RED 0x1f0000  // only using 5 bits to keep LEDs from getting too bright
#define GREEN 0x001f00
#define BLUE 0x00041f
#define YELLOW (RED|GREEN)
#define ORANGE 0x1f0800 
#define VIOLET (RED|BLUE)
#define AQUA (GREEN|BLUE)
#define WHITE (RED|GREEN|BLUE)

// constants for integer to float and float to integer conversion
#define MULT_16 2147483647
#define DIV_16 4.6566129e-10


#define DEBOUNCE 10   // debounce for buttons
#define GATE_DEBOUNCE 1  // some modules output a very short trigger so don't do a long debounce
#define TRIG_DEBOUNCE 1  // short trigger debounce too
#define CLOCK_DEBOUNCE 1 // short clock debounce
#define PARAMETERUPDATE 100  // some DaisySP models don't like values that jump around a lot so limit the changes

#define GATEHIGH -32767    // DAC values for gate levels
#define GATELOW 32767

// A/D values from CV inputs

#define CV1IN AIN0   // CV1 input - top jack
#define CV2IN AIN1   // CV2 input - middle jack
#define CV_AVERAGING 10  // A/D average over this many readings

// a/d values from pots
// the pots are "locked" when the parameter page changes
// this prevents an immediate change when we switch pages
// we unlock each pot and allow parameter values to change when there is a significant movement of the pot

#define AD_BITS 12
#define AD_RANGE 4096  // RP2350 has 12 bit A/D so lets use it
#define POT1_2  AIN3  // Pots 1 & 2 together on mux
#define POT3_4  AIN2  // Pots 3 & 4 ""
#define MIN_POT_CHANGE 100 // pot reading must change by this in order to register
#define POT_AVERAGING 5  // A/D average over this many readings
#define NUMPOTS 4


// Pot and CV sampling routines

uint16_t pot[NUMPOTS]; // pot A/D readings
bool potlock[NUMPOTS]; // when pots are locked it means they must change by MIN_POT_CHANGE to register


// sample the pots. potlock means apply hysteresis so we only change when the pot is moved significantly
void samplepots(void) {
  for (int i=0; i<NUMPOTS;++i) {
    uint val=0;
    digitalWrite(MUXCTL,!(i&1)); // set analog mux to correct pot
    for (int j=0; j<POT_AVERAGING;++j) val+=analogRead(POT1_2-((i&2)>>1)); // read the A/D a few times and average for a more stable value
    val=val/POT_AVERAGING;
    if (potlock[i]) {
      int delta=pot[i]-val;  // this needs to be done outside of the abs() function - see arduino abs() docs
      if (abs(delta) > MIN_POT_CHANGE) {
        potlock[i]=0;   // flag pot no longer locked
        pot[i]=val; // save the new reading
      }
    }
    else pot[i]=val; // pot is unlocked so save the reading
  }
}

// lock all pots. when locked they have to move significantly to change value. 
// prevents immediately setting new values when parameter set is changed with front panel button
void lockpots(void) {
  for (int i=0; i<NUMPOTS;++i) potlock[i]=1;
}

// sample the CV1 input. 
uint16_t sampleCV1(void) {
  uint16_t val=0;
  for (int16_t j=0; j<CV_AVERAGING;++j) val+=analogRead(CV1IN); // read the A/D a few times and average for a more stable value
  val=val/CV_AVERAGING;
  return val;
}

// sample the CV2 input. 
uint16_t sampleCV2(void) {
  uint16_t val=0;
  for (int16_t j=0; j<CV_AVERAGING;++j) val+=analogRead(CV2IN); // read the A/D a few times and average for a more stable value
  val=val/CV_AVERAGING;
  return val;
}

// like the map() function but maps to float values
float mapf(long x, long in_min, long in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
