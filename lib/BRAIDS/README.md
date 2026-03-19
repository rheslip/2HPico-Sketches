# arduinoMI
Ports of mutable intstruments eurorack code to arduino.

* braids works well on the RP2040/RP2350

This port was possible because of the work of Volker Boehm to bring MI modules to supercollider: https://github.com/v7b1/mi-UGens Very cool.

A simple sketch with no other io other than PWM pin 22 is located in examples. It cycles through all voices with random parameters every 3 seoncds.

To use the sketch, clone the STMLIB and BRAIDS repos to you ~/Arduino/libraries folder, open the BRAIDS example sketch change 

#define PWMOUT 22

to match whatever pin your using and

You should hear it step through the patches making small changes as it goes.


The STMLIB, BRAIDS directories can be placed in your Arduino/libraries/ folder and used as follows, for braids & plaits:
```
#pragma once

#include <STMLIB.h>
#include <BRAIDS.h>

#include "braids/envelope.h"
#include "braids/macro_oscillator.h"
#include "braids/quantizer.h"
#include "braids/signature_waveshaper.h"
#include "braids/quantizer_scales.h"
#include "braids/vco_jitter_source.h"


#define     MI_SAMPLERATE      96000.f
#define     BLOCK_SIZE          32      // --> macro_oscillator.h !
#define     SAMP_SCALE          (float)(1.0 / 32756.0)
#include <STMLIB.h>
#include <BRAIDS.h>
typedef struct
{
  braids::MacroOscillator *osc;

  float       samps[BLOCK_SIZE] ;
  int16_t     buffer[BLOCK_SIZE];
  uint8_t     sync_buffer[BLOCK_SIZE];

} PROCESS_CB_DATA ;

char shared_buffer[16384];
const size_t   kBlockSize = BLOCK_SIZE;

struct Unit {
  braids::Quantizer   *quantizer;
  braids::SignatureWaveshaper *ws;
  bool            last_trig;
  // resampler
  //SRC_STATE       *src_state;
  PROCESS_CB_DATA pd;
  float           *samples;
  float           ratio;
};

static long src_input_callback(void *cb_data, float **audio);
struct Unit voices[1];


```

See the included sketches.

