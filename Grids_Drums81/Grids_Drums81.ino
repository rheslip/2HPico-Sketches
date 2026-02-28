
// Copyright (c) 2021, Benjamin Rosenbach
// Copyright (c) 2026, Rich Heslip

// Based on Grids pattern generator, Copyright 2011 Émilie Gillet.
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

// R Heslip Drum Machine app for 2HPico eurack module Feb 2026
// based on Mutable Instruments Grids
// sequencer is derived from Phaserville DrumMap applet, which is derived from Grids
// sample player is same code and wav to C header tool used in a lot of my other projects
// Feb 7/26 - added no interpolation and manual reset features
// expanded grid to 9x9

// top jack - clock input - clocks on +ve edge
// middle jack - reset input to sync with other sequencers - resets on +ve edge
// bottom jack - audio out

// button - click to advance to next page
// button - double click on page 1 to toggle drum map interpolation off/on
// button - hold to manually reset to first step

// page 1 parameters - Red LED if interpolation is on, Orange LED is interpolation is off
// Pot 1 - Drum Map X
// pot 2 - Drum Map Y
// pot 3 - Chaos (adds variations)
// pot 4 - Probability of a random sample on channel 4

// page 2 parameters - Green LED
// Pot 1 - Channel 1 Fill
// pot 2 - Channel 2 Fill
// pot 3 - Channel 3 Fill
// pot 4 - Channel 4 Fill

// page 3 parameters Aqua LED
// Pot 1 - Channel 1 Sample
// Pot 2 - Channel 2 Sample
// Pot 3 - Channel 3 Sample
// Pot 4 - Channel 4 Sample

#include "2HPico.h"
#include <I2S.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>
#include "grids9x9_resources.h" // 4 channel map - 81 patterns
#include "pico/multicore.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

#define MONITOR_CPU1  // define to enable 2nd core monitoring
//#define DEBUG   // comment out to remove debug code

//#define SAMPLERATE 11025 
#define SAMPLERATE 22050  // saves CPU cycles
//#define SAMPLERATE 44100


Adafruit_NeoPixel LEDS(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

I2S DAC(OUTPUT);  // 

// constants for integer to float and float to integer conversion
#define MULT_16 2147483647
#define DIV_16 4.6566129e-10

// Grids sequencer stuff
#define NUM_CHANNELS 4 // 3 if you are using normal grids maps, 4 requires modded drum map
#define MAX_STEPS 32
#define NON_ACCENT_LEVEL 64  // volume multiplier
#define ACCENT_LEVEL 127
static constexpr int MAX_VAL = 255;
uint8_t randomness[NUM_CHANNELS] = {0, 0, 0, 0};
int16_t fill[NUM_CHANNELS] = {128, 128,128,128};
int8_t step=0;
int16_t x=0;
int16_t y=0;
int16_t chaos=0;
int16_t last_ch_sample=3; // saves sample setting of ch 3 when random voice is enabled
int16_t last_ch_randomness=0;

#define CLOCKIN TRIGGER  // top jack is clock
#define RESETIN AIN1   // middle jack used to reset the sequencer

bool clocked=0;  // keeps track of clock state
bool resetedge=0;  // keeps track of reset state
bool interpolate=1;  // true to interpolate drum patterns

// clickbutton library is used to detect clicks, doubleclicks, holds etc
ClickButton button1 (BUTTON1);
#define BUTTON_TIMER_MICROS 10000 // 100hz for button

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

#define NUMUISTATES 3
enum UIstates {SET1,SET2,SET3} ;
uint8_t UIstate=SET1;
uint32_t clocktimer,clockdebouncetimer,resetdebouncetimer,ledtimer;

#define LEDOFF 25 // LED trigger flash time 

#define NUM_VOICES 4  // 
struct voice_t {
  int16_t sample;   // menusystem requires ints
  int8_t level;   // 0-1000, shown as a float hown as a float -1 to 1.0
} voice[NUM_VOICES] = {
  0,      // default voice 0 assignment 
  127,    // max volume

  1,      // default voice 1 assignment 
  127,    // max volume
  
  2,    // default voice 2 assignment 
  127,    // max volume
  
  3,    // default voice 3 assignment 
  127,   // max volume
};  


// we can have an arbitrary number of samples but you will run out of memory (or CPU) at some point
// sound sample files must be 22khz 16 bit signed PCM format - see the sample include files for examples
// sample files are compiled into arrays and stored in program flash
// if your sketch is too big you may have to fiddle with the flash partition scheme to enable more of the flash to be used for program space

// the header files can be auto generated by the wav2header utility
// put your 22khz or 44khz PCM wav files in a sample directory, run the utility and it will generate all the header files

// include only ONE sample header file
//#include "808samples/samples.h" // 808 sounds
//#include "Angular_Jungle_Set/samples.h"   // Jungle soundfont set - great!
//#include "Angular_Techno_Set/samples.h"   // Techno
//#include "Acoustic3/samples.h"   // acoustic drums 
//#include "Pico_kit/samples.h"   // assorted samples
//#include "testkit/samples.h"   // assorted samples
//#include "EDM_kits/samples.h"   // Techno, Pop, Trap, House
#include "House/samples.h"   // House 808 909 style kit

#define NUM_SAMPLES (sizeof(sample)/sizeof(sample_t))


// Phaserville code, more or less lifted from Grids - modded for 9x9 matrix

uint8_t ReadDrumMap(uint8_t step, uint8_t part, uint8_t x, uint8_t y) {
  uint8_t i = x >> 5;
  uint8_t j = y >> 5;
  const uint8_t* a_map = grids::drum_map[i][j];
  const uint8_t* b_map = grids::drum_map[i + 1][j];
  const uint8_t* c_map = grids::drum_map[i][j + 1];
  const uint8_t* d_map = grids::drum_map[i + 1][j + 1];
  uint8_t offset = (part * MAX_STEPS) + step;
  uint8_t a = a_map[offset];
  uint8_t b = b_map[offset];
  uint8_t c = c_map[offset];
  uint8_t d = d_map[offset];
  uint8_t quad_x = x << 3;
  uint8_t quad_y = y << 3;
  // return U8Mix(U8Mix(a, b, x << 2), U8Mix(c, d, x << 2), y << 2);
  // U8Mix returns b * x + a * (255 - x) >> 8
  uint8_t ab_fade = (b * quad_x + a * (255 - quad_x)) >> 8;
  uint8_t cd_fade = (d * quad_x + c * (255 - quad_x)) >> 8;
  return (cd_fade * quad_y + ab_fade * (255 - quad_y)) >> 8;
}


// read drum pattern without interpolation between adjacent patterns

uint8_t ReadDrumPattern(uint8_t step, uint8_t part, uint8_t x, uint8_t y) {
  uint8_t i = map(x,0,MAX_VAL,0,8);
  uint8_t j = map(y,0,MAX_VAL,0,8);
  const uint8_t* a_map = grids::drum_map[i][j];
  uint8_t offset = (part * MAX_STEPS) + step;
  uint8_t a = a_map[offset];
  return a;
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

  pinMode(CLOCKIN,INPUT); // gate/trigger in used for clock
  pinMode(RESETIN,INPUT); // 2nd jack in used for reset
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

  alarm_in_us(BUTTON_TIMER_MICROS); // start the button IRQ

#ifdef DEBUG  
  Serial.println("finished setup");  
#endif
  clocktimer=millis(); // initial clock measurement
}



void loop() {

  ClickButton::Button b=button1.getButton();
    switch (b) {
    case ClickButton::Clicked:
      ++UIstate;
      if (UIstate >= NUMUISTATES) UIstate=SET1;
      lockpots();
      break;
    case ClickButton::DoubleClicked:
      if (UIstate==SET1) interpolate=!interpolate; // flips interpolation on and off
      break;
    case ClickButton::Held:  // hold to reset sequencer
      step=0;
      break;
    default:
      break;
  }

  samplepots();

//  int16_t cv=(AD_RANGE-sampleCV2()); // CV in is inverted
//  cv=map(cv,0,AD_RANGE-1,128,-128); // make it bipolar 
//  Serial.printf("cv %d\n", cv);

// set parameters from panel pots
// 
  switch (UIstate) {
    case SET1:
      if (interpolate) LEDS.setPixelColor(0, RED); 
      else LEDS.setPixelColor(0, ORANGE);
      if (!potlock[0]) x=map(pot[0],0,AD_RANGE-1,0,MAX_VAL); // top pot on the panel
      if (!potlock[1]) y=map(pot[1],0,AD_RANGE-1,0,MAX_VAL);  // 
      if (!potlock[2]) chaos=map(pot[2],0,AD_RANGE-1,0,MAX_VAL);
      if (!potlock[3]) last_ch_randomness=map(pot[3],0,AD_RANGE-1,0,100); // random setting for last channel sample selection
      break;
    case SET2:
      LEDS.setPixelColor(0, GREEN);
      if (!potlock[0]) fill[0]=map(pot[0],0,AD_RANGE-1,0,MAX_VAL); // top pot on the panel
      if (!potlock[1]) fill[1]=map(pot[1],0,AD_RANGE-1,0,MAX_VAL);  // 
      if (!potlock[2]) fill[2]=map(pot[2],0,AD_RANGE-1,0,MAX_VAL);
      if (!potlock[3]) fill[3]=map(pot[3],0,AD_RANGE-1,0,MAX_VAL); 
      break;
    case SET3:
      LEDS.setPixelColor(0, AQUA);
      if (!potlock[0]) voice[0].sample=map(pot[0],0,AD_RANGE-1,0,NUM_SAMPLES-1); // top pot on the panel
      if (!potlock[1]) voice[1].sample=map(pot[1],0,AD_RANGE-1,0,NUM_SAMPLES-1);  // 
      if (!potlock[2]) voice[2].sample=map(pot[2],0,AD_RANGE-1,0,NUM_SAMPLES-1);
      if (!potlock[3]) voice[3].sample=last_ch_sample=map(pot[3],0,AD_RANGE-1,0,NUM_SAMPLES-1); 
      break;
    default:
      break;
  }

  if (!digitalRead(RESETIN))  { // look for reset - reset input is inverted
    if (((millis()-resetdebouncetimer) > CLOCK_DEBOUNCE) && !resetedge) {  // true on rising edge
      resetedge=1;
      step=0; // reset the sequencer
    }
  }
  else {
    resetedge=0;
    resetdebouncetimer=millis();
  }


  if (!digitalRead(CLOCKIN)) {  // look for rising edge of clock input which is inverted
    if (((millis()-clockdebouncetimer) > CLOCK_DEBOUNCE) && !clocked) {  // true if we have a debounced clock rising edge
      clocked=1;
      LEDS.setPixelColor(0,0 ); // show clock as a LED off flash
      LEDS.show();  // update LED
      ledtimer=millis(); // start led off timer

      if (step == 0) {
          for (int i = 0; i < NUM_CHANNELS; i++) {
              randomness[i] = random(0, chaos >> 2); // 0-63
          }
      }

      for (int16_t ch=0; ch< NUM_CHANNELS;++ch) {
        // accent on ch 1 will be for whatever part ch 0 is set to
        //uint8_t part = (ch == 1 && mode[ch] == 3) ? mode[0] : mode[ch];
        uint8_t part = ch;
        int level;
        if (interpolate) level = ReadDrumMap(step, part, x, y);
        else level=ReadDrumPattern(step, part, x, y);
        level = constrain(level + randomness[part], 0, MAX_VAL);
        // use ch 0 fill if ch 1 is in accent mode
        //uint8_t threshold = (ch == 1 && mode[ch] == 3) ? ~_fill[0] : ~_fill[ch];
        uint8_t threshold = ~fill[ch];
        //int16_t threshold = ~(fill[ch]+cv);  // cv in moduluates all fills
        if (level > threshold) {
          if (ch==NUM_CHANNELS-1) {
            if (last_ch_randomness > random(0,99)) voice[ch].sample=random(0,NUM_SAMPLES-1); // randomize last channel 
            else voice[ch].sample=last_ch_sample;  // or set it back to what it was
          }
          if (level > 192) sample[voice[ch].sample].play_volume=ACCENT_LEVEL;
          else sample[voice[ch].sample].play_volume=NON_ACCENT_LEVEL;
          sample[voice[ch].sample].sampleindex=0; // trigger sample for this track

        }
      }
      if (++step >= MAX_STEPS) step = 0;
    }
  }
  else {   
      clocked=0;
      clockdebouncetimer=millis();
  }


  if ((millis()-ledtimer) > LEDOFF ) LEDS.show();  // update LEDs only if not doing off flash

}

// second core setup
// second core is dedicated to sample processing
void setup1() {
delay (1000); // wait for main core to start up peripherals
}

// process audio samples
void loop1(){
  int32_t samplesum=0;
#ifdef MONITOR_CPU1  
  digitalWrite(CPU_USE,0); // low - CPU not busy
#endif

  for (int i=0; i< NUM_SAMPLES;++i) {  // look for samples that are playing, scale their volume, and add them up
    if (sample[i].sampleindex < sample[i].samplesize) samplesum+=(int32_t)(sample[i].samplearray[sample[i].sampleindex++]*sample[i].play_volume);  // thats a mouthful!
  }
  samplesum=samplesum>>7;  // adjust for play_volume multiply above
  if  (samplesum>32767) samplesum=32767; // clip if sample sum is too large
  if  (samplesum<-32767) samplesum=-32767;

 // write samples to DMA buffer - this is a blocking call so it stalls when buffer is full
	DAC.write(int16_t(samplesum)); // left
	DAC.write(int16_t(samplesum)); // right

#ifdef MONITOR_CPU1
  digitalWrite(CPU_USE,1); // hi = CPU busy
#endif
}





