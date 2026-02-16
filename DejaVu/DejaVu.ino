
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

// DejaVu sequencer - sort of like Moog Labyrinth + a unique "DejaVu" control
// R Heslip for new 2HP hardware Dec 2025

// this is still a bit of a work in progress. 
// sometimes the sequencer outputs nothing - turn up the note and gate probabilities and be patient
// instead of outputting a gate every clock it combines active gates into one longer gate to give more rhythmic variety
// the idea is to turn DejaVu to zero (fully CCW) and fiddle with the other controls till you hear something to capture
// when you turn up the DejaVu pot it will capture the current sequence and keep throwing it back into the sequencer at a rate determined by the pot setting
// instead of being totally random, DejaVu captures a "theme" that gets randomized, repeated, randomized etc. The note and gate probabilities determine how much randomization takes place

// top jack - clock input - clocks on +ve edge
// middle jack - must be jumpered for CV out - Gate output
// bottom jack - CV out

// button - click to advance to next UI page

// page 1 parameters - Red LED 
// Pot 1 - Probability of changing notes
// pot 2 - Probability of changing gates
// pot 3 - Note range
// pot 4 - "DejaVu" - when set to zero it does nothing. when turned up it captures the current sequence and resets the sequencer to that sequence every N cycles. N=1 to 16 - increase by rotating CW

// page 2 parameters - Green LED
// Pot 1 - Scale - currently only 4 scales Major Penatonic, Major, Minor Penatonic, Minor
// pot 2 - 
// pot 3 - 
// pot 4 - CV offset



#include "2HPico.h"
//#include "io.h"
#include <I2S.h>
#include <Adafruit_NeoPixel.h>
//#include "RPi_Pico_TimerInterrupt.h"
#include <math.h>
//#include "scales.h"

#include "pico/multicore.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

#define MONITOR_CPU1  // define to enable 2nd core monitoring

//#define SAMPLERATE 11025 
#define SAMPLERATE 22050  // saves CPU cycles
//#define SAMPLERATE 44100

#define NUMPIXELS 1 // 
#define RED 0x1f0000  // only using 5 bits to keep LEDs from getting too bright
#define GREEN 0x001f00
#define BLUE 0x00001f
#define ORANGE (RED|GREEN)
#define VIOLET (RED|BLUE)
#define AQUA (GREEN|BLUE)

Adafruit_NeoPixel LEDS(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

I2S DAC(OUTPUT);  // 

#define DEBUG   // comment out to remove debug code

// constants for integer to float and float to integer conversion
#define MULT_16 2147483647
#define DIV_16 4.6566129e-10

// sequencer stuff
#define MAX_STEPS 16
#define NOTERANGE 36  // 3 octave range seems reasonable
#define CLOCKIN TRIGGER  // top jack is clock
#define GATEHIGH -32767    // DAC values for gate levels
#define GATELOW 32767


#define CV1IN_VOLT 580.6  // a/d count per volt - trim for V/octave
#define CV1IN AIN0   // CV1 input - top jack
#define CV2IN AIN1   // CV2 input - top jack
#define CV_AVERAGING 10  // A/D average over this many readings
#define CVOUT_VOLT 6554 // D/A count per volt - nominally +-5v range for -+32767 DAC values- trim for V/octave out
#define CVOUTMIN -2*CVOUT_VOLT  // lowest output CV ie MIDI note 0

int8_t notes[MAX_STEPS]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool gates[MAX_STEPS]={1,1,1,0,1,0,0,0,1,0,0,0,1,0,0,0};
int8_t savednotes[MAX_STEPS];
bool savedgates[MAX_STEPS];
int16_t gateout=GATELOW;      // sent to right dac channel
int16_t cvout=CVOUTMIN;  // sent to left DAC channel
int16_t cvoffset=12000; // set by UI
int8_t stepindex=0;
int8_t laststep=15;
int8_t noterange=7;
int16_t scale=0;
int8_t noteprob=10;      // probability of changing notes 0-99
int8_t gateprob=10;      // "" gates
int8_t clockdivideby=1;  // divide input clock by this
int8_t clockdivider=1;   // counts down clocks
int8_t sequencecounter=0; // counts sequence iterations
int8_t restorecount=8;   // restore the sequence after this many iterations

bool clocked=0;  // keeps track of clock state
bool button=0;  // keeps track of button state

#define NUMUISTATES 2
enum UIstates {SET1,SET2} ;
uint8_t UIstate=SET1;
uint32_t buttontimer,clocktimer,clockperiod,clockdebouncetimer,ledtimer, gatetimer, gatelength;

#define LEDOFF 100 // LED trigger flash time 



// requantizes note array to current scale setting
void requantizenotes(void) {
  for (int16_t i=0; i< MAX_STEPS;++i) notes[i]=quantize(notes[i],scales[scale],0);
}

// save current sequence
void savesequence(void) {
  for (int16_t i=0;i<MAX_STEPS;++i) {
    savednotes[i]=notes[i];
    savedgates[i]=gates[i];
  }
}

// restore saved sequence
void restoresequence(void) {
  for (int16_t i=0;i<MAX_STEPS;++i) {
    notes[i]=savednotes[i];
    gates[i]=savedgates[i];
  }
}


void setup() { 
  Serial.begin(115200);

#ifdef DEBUG

  Serial.println("starting setup");  
#endif

// set up I/O pins
 
#ifdef MONITOR_CPU1 // for monitoring 2nd core CPU usage
  pinMode(CPU_USE,OUTPUT); // hi = CPU busy
#endif 

  pinMode(TRIGGER,INPUT_PULLUP); // gate/trigger in
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


  // Enable the AudioShield
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
  clocktimer=millis(); // initial clock measurement
}



void loop() {

  if (!digitalRead(BUTTON1)) {
    if (((millis()-buttontimer) > DEBOUNCE) && !button) {  // if button pressed advance to next parameter set
      button=1;  
      ++UIstate;
      if (UIstate >= NUMUISTATES) UIstate=SET1;
      lockpots();
  //    Serial.printf("state %d %d\n",UIstate, sizeof(UIstates)); 
    }
  }
  else {
    buttontimer=millis();
    button=0;
  }

  samplepots();

// set parameters from panel pots
// 
  switch (UIstate) {
    case SET1:
      LEDS.setPixelColor(0, RED); 
      if (!potlock[0]) noteprob=map(pot[0],0,AD_RANGE-1,0,100); // top pot on the panel
      if (!potlock[1]) gateprob=map(pot[1],0,AD_RANGE-1,0,100);  // 
      if (!potlock[2]) noterange=map(pot[2],0,AD_RANGE-1,0,NOTERANGE);
      if (!potlock[3]) {
        restorecount=map(pot[3],0,AD_RANGE-1,0,8); // top pot on the panel 
        //if ((restorecount==0) && (stepindex==0)) savesequence(); // save sequence if value is zero
        if ((restorecount==0) ) savesequence(); // save sequence if value is zero
      }      
      break;
    case SET2:
      LEDS.setPixelColor(0, GREEN);
      if (!potlock[0]) {
        scale=map(pot[0],0,AD_RANGE,0,4);  // too many scales gets hard to discern by ear 
        requantizenotes();
      }
                                                            // *** should also requantize the sequence here when scale changes to purge out of scale notes
      // if (!potlock[0]) clockdivideby=map(pot[0],0,AD_RANGE-1,1,16); // clock dvider works but should probably be limited to /2/4/8 ?
      if (!potlock[3]) cvoffset=map(pot[3],0,AD_RANGE-1,CVOUTMIN,32767); 
      break;
    default:
      break;
  }

//  if ((noteprob==0) && (gateprob==0)) savesequence();  // if sequence is locked, save it

  if (!digitalRead(CLOCKIN)) {  // look for rising edge of clock input which is inverted
    if (((millis()-clockdebouncetimer) > CLOCK_DEBOUNCE) && !clocked) {  // true if we have a debounced clock rising edge
      --clockdivider;
      clocked=1;
      if (clockdivider <=0) {       
        clockdivider=clockdivideby;
        clockperiod=millis()-clocktimer; // measure clock so we can set gate time relative to clock period
        clocktimer=millis();

        ++stepindex; // advance sequencer
        if (stepindex > laststep) {
          stepindex=0;
          ++sequencecounter;
          
          for (int16_t i=0;i<MAX_STEPS;++i) {  // randomize the sequence according to the probabilities set   
            int p=random(100);        
            if (p < noteprob) notes[i]=quantize(random(noterange),scales[scale],0);  //root is always MIDI note 0 - CV offset actually sets output pitch 
            if (p < gateprob) gates[i]=(bool)random(2);
          }
        }
        
        if ((restorecount >1) && (sequencecounter > restorecount)) { // see if we are restoring sequence for a repeating theme
          sequencecounter=0;
          restoresequence();
        }

        if (gates[stepindex]) {
          cvout=-(notes[stepindex]*(CVOUT_VOLT/12)+CVOUTMIN+cvoffset); // 1v per octave. note numbers are MIDI style 0-128. DAC out is inverted. change CV only if there is a gate
          gateout=GATEHIGH;
          if (gates[stepindex] && gates[(stepindex+1)%MAX_STEPS] && (notes[stepindex]==notes[(stepindex+1)%MAX_STEPS])) { // if two consective notes are the same, tie
            int16_t gatecount=0;     // if all gates are high skip it or we'll tie all notes
            for (int16_t i=0;i<MAX_STEPS;++i) if (gates[i]) ++gatecount;
            if (gatecount != (MAX_STEPS)) gatelength=clockperiod +clockperiod/2; // if 2 consecutive notes are the same, tie
          }
          else gatelength=clockperiod/2; // *** gatelength could be adjustable
          gatetimer=millis(); // start gate timer
          LEDS.setPixelColor(0,0 ); // show gate as a LED off flash
          LEDS.show();  // update LED
          ledtimer=millis(); // start led off timer
 //         Serial.printf("scale %s note %s \n",scalenames[scale],notenames[notes[stepindex]%12]);
        }
        else gateout=GATELOW;
      }
    }
  }
  else {   
      clocked=0;
      clockdebouncetimer=millis();
  }

  if ((millis()-gatetimer) > gatelength) gateout=GATELOW;  // turn off gate after gate length

  if ((millis()-ledtimer) > LEDOFF ) LEDS.show();  // update LEDs only if not doing off flash
}

// second core setup
// second core is dedicated to sample processing
void setup1() {
delay (1000); // wait for main core to start up peripherals
}

// process audio samples
void loop1(){

#ifdef MONITOR_CPU1  
  digitalWrite(CPU_USE,0); // low - CPU not busy
#endif
 // write samples to DMA buffer - this is a blocking call so it stalls when buffer is full
	DAC.write(int16_t(cvout)); // left
	DAC.write(int16_t(gateout)); // right

#ifdef MONITOR_CPU1
  digitalWrite(CPU_USE,1); // hi = CPU busy
#endif
}





