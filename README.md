# Sketches for the 2HPico-Eurorack-Module-Hardware

Some sample sketches that run on the 2HPico eurorack module

You must have Arduino 2.xx installed with the Pico board support package https://github.com/earlephilhower/arduino-pico

Select board type as Raspberry Pi Pico or Raspberry Pico Pico 2 depending on what board you used when building the module. Some sketches require overclocking - check the readme files.

Dependencies:

2HPico library included in this repository - install it in your Arduino/Libraries directory

Adafruit Neopixel library

Some sketches use my fork of ElectroSmith's DaisySP library https://github.com/rheslip/DaisySP_Teensy

Plaits sketch requires PLAITS and STMLIB libraries included in this repository

PlaitsFM sketch requires 6opfm and STMLIB libraries included in this repository

Pico_Audio_Braids requires the pico-audio library from my main repository https://github.com/rheslip/pico-audio and the Synth_Braids library in this repository

	Synth_Braids library from https://codeberg.org/positionhigh/MicroDexed-touch/src/branch/main/third-party

