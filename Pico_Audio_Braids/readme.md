# Pico_Audio_Braids sketch for 2HPico module 

See the comments at the op of the sketch for usage instructions

March 14/2026 - initial commit

This sketch uses the pico-audio framework which is a port of Teensy Audio to the RP2350. It has some advantages over DaisySP - more efficient use of memory, 16 bit fixed point math and use of ARM Neon DSP instructions which can be a considerable speedup over DaisySP floating point. Its main disadvantage is a somewhat more complex programming model which can get pretty klunky when there are a lot of audio processing objects.

My fork of pico-audio has some new features and bug fixes relative to the main repository. A new 32 bit I2S input driver was added for the PCM1808 on the 2HPico DSP - it may not work with other I2S input devices. Please use https://github.com/rheslip/pico-audio for 2HPico and 2HPico DSP programming.

Also required for the Pico_Audio_Braids sketch is the Synth_Braids library in this repository. Synth_Braids originated at https://codeberg.org/positionhigh/MicroDexed-touch/src/branch/main/third-party. 

Issues:

There seems to be stability issues with some of the models. This was noted in the original Synth_Braids repository.