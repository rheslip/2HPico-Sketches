# arduinoMI
Ports of mutable intstruments eurorack code to arduino.

* plaits works fine on the RP2350 only

This port was possible because of the work of Volker Boehm to bring MI modules to supercollider: https://github.com/v7b1/mi-UGens Very cool.

A simple sketch with no other io other than PWM pin 22 is located in examples. It cycles through all voices with random parameters every 3 seoncds.

To use the sketch, clone the STMLIB and PLAITS repos to ~/Arduino/libraries folder, open the PLAITS example sketch change 

#define PWMOUT 22

to match whatever pin your using and

choose 250mHz overclocking (well, 200 should do it too, but, 250 is stable) and optimize. Then install it on your pico. 

You should hear it step through the patches making small changes as it goes.


The STMLIB, and PLAITS directories can be placed in your Arduino/libraries/ folder and used as follows, for braids & plaits:
```

// plaits dsp
#include <STMLIB.h>
#include <PLAITS.h>

#include "plaits/dsp/dsp.h"
#include "plaits/dsp/voice.h"

plaits::Modulations modulations;
plaits::Patch patch;
plaits::Voice voice;

char shared_buffer[16384];
stmlib::BufferAllocator allocator;

//float a0 = (440.0 / 8.0) / kSampleRate; //48000.00;
const size_t   kBlockSize = plaits::kBlockSize;

plaits::Voice::Frame outputPlaits[ plaits::kBlockSize];

struct Unit {
  plaits::Voice       *voice_;
  plaits::Modulations modulations;
  plaits::Patch       patch;
  float               transposition_;
  float               octave_;
  short               trigger_connected;
  short               trigger_toggle;

  char                *shared_buffer;
  void                *info_out;
  bool                prev_trig;
  float               sr;
  int                 sigvs;
};

```


